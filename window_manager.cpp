#include "window_manager.hpp"
#include "event_utils.hpp"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <glog/logging.h>
#include <memory>

bool WindowManager::wm_detected_;

std::unique_ptr<WindowManager> WindowManager::Create() {

  Display *display = XOpenDisplay(nullptr);
  if (display == nullptr) {
    LOG(ERROR) << "Failed to open X display" << XDisplayName(nullptr);
    return nullptr;
  }

  return std::unique_ptr<WindowManager>(new WindowManager(display));
}

WindowManager::WindowManager(Display *display)
    : display_(display), root_(DefaultRootWindow(display_)) {}

WindowManager::~WindowManager() { XCloseDisplay(display_); }

void WindowManager::Run() {
  // Initialization
  //  Selection of events on root window. We use a special error handler to exit
  //  gracefully if another wm is running.
  wm_detected_ = false;
  XSetErrorHandler(&WindowManager::onWmDetected);
  XSelectInput(display_, root_,
               SubstructureRedirectMask | SubstructureNotifyMask);
  XSync(display_, false);

  if (wm_detected_) {
    LOG(ERROR) << "Detected another window manager on display"
               << XDisplayString(display_);
    return;
  }
  XSetErrorHandler(&WindowManager::OnXError);

  // Grab the  X server to prevent windows from changing under us while framing
  XGrabServer(display_);
  Window returned_root, returned_parent;

  Window *top_level_windows;

  unsigned int num_top_level_windows;
  CHECK(XQueryTree(display_, root_, &returned_root, &returned_parent,
                   &top_level_windows, &num_top_level_windows));
  CHECK_EQ(returned_root, root_);

  for (unsigned int i = 0; i < num_top_level_windows; i++) {
    // True here means that the window was created before window manager
    Frame(top_level_windows[i], true);
  }

  // Free the top level window array
  XFree(top_level_windows);

  // Ungrab the x server
  XUngrabServer(display_);

  // Main  event loop
  for (;;) {
    // Get the next even
    XEvent e;
    XNextEvent(display_, &e);
    LOG(INFO) << "Received event: " << window_utils::ToString(e);

    // Dispatch our events
    switch (e.type) {
    case CreateNotify:
      OnCreateNotify(e.xcreatewindow);
      break;

    case DestroyNotify:
      OnDestroyNotify(e.xdestroywindow);
      break;
    case ReparentNotify:
      OnReparentNotify(e.xreparent);
      break;

    case ConfigureNotify:
      OnConfigureNotify(e.xconfigure);
      break;
    case UnmapNotify:
      OnUnmapNotify(e.xunmap);
      break;

    case MapRequest:
      OnMapRequest(e.xmaprequest);
      break;
    case ConfigureRequest:
      OnConfigureRequest(e.xconfigurerequest);
      break;

    case ButtonPress:
      OnButtonPress(e.xbutton);
      break;

    case ButtonRelease:
      OnButtonRelease(e.xbutton);
      break;

    case MotionNotify:

      // Skip any pending motion events
      while (XCheckTypedWindowEvent(display_, e.xmotion.window, MotionNotify,
                                    &e)) {
      }
      OnMotionNotify(e.xmotion);

      break;

    case KeyPress:
      OnKeyPress(e.xkey);
      break;
    case KeyRelease:
      OnKeyRelease(e.xkey);
      break;

    default:
      LOG(WARNING) << "Ignored event" << window_utils::ToString(e);
    }
  }
}
void WindowManager::OnCreateNotify(const XCreateWindowEvent &e) {}
void WindowManager::OnConfigureNotify(const XConfigureEvent &e) {}
void WindowManager::OnReparentNotify(const XReparentEvent &e) {}
void WindowManager::OnUnmapNotify(const XUnmapEvent &e) {
  // Check if window is client window we manage so that we can unframe it upon
  // unmap notify. We will receive an unmap notify request for a client window
  // we destroyed ourselves
  if (!clients_.count(e.window)) {
    LOG(INFO) << "Ignore unmap notify for non-client window" << e.window;
    return;
  }

  // ignore this event if it was triggered by reparenting a window that was
  // mapped before the  window manager started. the event arg of  a window we
  // maintain should be set to a frame window we maintain. Only an unmap notify
  // event triggered by reparenting a pre-existing will have this attribute set
  // to the root window.

  if (e.event == root_) {
    LOG(INFO) << "Ignoring unmap notify for reparenting pre-existing window "
              << e.window;

    return;
  }

  UnFrame(e.window);
}

void WindowManager::OnDestroyNotify(const XDestroyWindowEvent &e) {}

void WindowManager::OnConfigureRequest(const XConfigureRequestEvent &e) {

  XWindowChanges changes;

  changes.x = e.x;
  changes.y = e.y;
  changes.width = e.width;
  changes.height = e.height;
  changes.border_width = e.border_width;
  changes.sibling = e.above;
  changes.stack_mode = e.detail;

  if (clients_.count(e.window)) {
    const Window frame = clients_[e.window];
    XConfigureWindow(display_, frame, e.value_mask, &changes);
    LOG(INFO) << "Resize [" << frame << "]" << "to"
              << window_utils::Size<int>(e.width, e.height);
  }

  // Grant the request
  XConfigureWindow(display_, e.window, e.value_mask, &changes);
  LOG(INFO) << "Resize " << e.window << " to "
            << window_utils::Size<int>(e.width, e.height);
}

void WindowManager::OnMapRequest(const XMapRequestEvent &e) {
  Frame(e.window, false);

  XMapWindow(display_, e.window);
}

void WindowManager::OnButtonPress(const XButtonEvent &e) {
  CHECK(clients_.count(e.window));
  const Window frame = clients_[e.window];

  // initial cursor position
  drag_start_position_ = window_utils::Position<int>(e.x_root, e.y_root);
  // Saving the initial window information
  Window returned_root;
  int x, y;
  unsigned int width, height, border_width, depth;
  CHECK(XGetGeometry(display_, frame, &returned_root, &x, &y, &width, &height,
                     &border_width, &depth));

  drag_start_frame_position_ = window_utils::Position<int>(x, y);
  drag_start_frame_size_ = window_utils::Size<int>(width, height);

  // Raise the clicked window to the top

  XRaiseWindow(display_, frame);
}

void WindowManager::Frame(Window w, bool was_created_before_window_manager) {
  const unsigned int BORDER_WIDTH = 4;
  const unsigned long BORDER_COLOR = 0xff0000;
  const unsigned int BG_COLOR = 0x0000ff;
  // Retrieve window Attributes to our Frame

  XWindowAttributes x_window_attrs;
  CHECK(XGetWindowAttributes(display_, w, &x_window_attrs));

  // If window existed before our wm started we should frame it only if it is
  // visible and doesnt set override redirect

  if (was_created_before_window_manager) {
    if (x_window_attrs.override_redirect ||
        x_window_attrs.map_state != IsViewable) {
      return;
    }
  }

  // Frame creation

  const Window frame = XCreateSimpleWindow(
      display_, root_, x_window_attrs.x, x_window_attrs.y, x_window_attrs.width,
      x_window_attrs.height, BORDER_WIDTH, BORDER_COLOR, BG_COLOR);

  // SELECT events on frame
  XSelectInput(display_, frame,
               SubstructureRedirectMask | SubstructureNotifyMask);
  // Add client to save set so that it will be restored and kept alive incase of
  // a crash
  XAddToSaveSet(display_, w);

  // Reparent the client window to the frame
  //  (0,0) offset of client window within the frame
  XReparentWindow(display_, w, frame, 0, 0);
  // Map frame onto display
  XMapWindow(display_, frame);

  clients_[w] = frame;
  // Grab events for window managament actions on client windows

  // a. Move windows with alt+left btn
  XGrabButton(display_, Button1, Mod1Mask, w, false,
              ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
              GrabModeAsync, GrabModeAsync, None, None);

  // b. Move windows with alt+right btn

  XGrabButton(display_, Button3, Mod1Mask, w, false,
              ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
              GrabModeAsync, GrabModeAsync, None, None);

  // c. kill windows with alt+f4

  XGrabKey(display_, XKeysymToKeycode(display_, XK_F4), Mod1Mask, w, false,
           GrabModeAsync, GrabModeAsync);

  // d. Switch windows with alt+tab
  XGrabKey(display_, XKeysymToKeycode(display_, XK_Tab), Mod1Mask, w, false,
           GrabModeAsync, GrabModeAsync);

  LOG(INFO) << "Framed window " << w << " [" << frame << "]";
}

void WindowManager::UnFrame(Window w) {
  const Window frame = clients_[w];
  XUnmapWindow(display_, frame);

  XReparentWindow(display_, w, root_, 0, 0);
  // Remove from save set since the window is now unrelated to us
  XRemoveFromSaveSet(display_, w);
  XDestroyWindow(display_, frame);

  clients_.erase(w);

  LOG(INFO) << "Unframed window" << w << " [" << frame << "]";
}

int WindowManager::onWmDetected(Display *display, XErrorEvent *e) {
  // If we have an already running wm, the error code from XselectInput is
  // BadAccess. We do not expect this handler to receive any other errors.
  CHECK_EQ(static_cast<int>(e->error_code), BadAccess);

  wm_detected_ = true;
  return 0;
}
int WindowManager::OnXError(Display *display, XErrorEvent *e) {

  LOG(ERROR) << "X11 error was encountered. Error code: " << e->error_code
             << "Request code: " << e->request_code;

  return 0;
}
