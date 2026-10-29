#include <X11/X.h>
#include <X11/keysym.h>
#include <unordered_map>
extern "C" {
#include <X11/Xlib.h>
}
#include <memory>

class WindowManager {
public:
  // Establish connection  to x server and create a window manager instance
  static std::unique_ptr<WindowManager> Create();

  ~WindowManager();
  void Run();

private:
  // Invoked internally by Create().
  WindowManager(Display *display);

  // Handle to the underlying xlib display struct
  Display *display_;

  // Handle to root window
  const Window root_;

  // Maps top-level windows to their frame windows.
  std::unordered_map<Window, Window> clients_;

  // Event handlers
  void OnCreateNotify(const XCreateWindowEvent &e);
  void OnConfigureNotify(const XConfigureEvent &e);
  void OnUnmapNotify(const XUnmapEvent &e);
  void OnConfigureRequest(const XConfigureRequestEvent &e);
  void OnMapRequest(const XMapRequestEvent &e);
  void OnReparentNotify(const XReparentEvent &e);
  void OnDestroyNotify(const XDestroyWindowEvent &e);
  // Helper functions
  void Frame(Window w);
  void UnFrame(Window w); // Xlib error handler. Must be static as its address
                          // is passed to xlib
  static int OnXError(Display *display, XErrorEvent *e);
  // Used to determine whether another window manager is running. Set as the
  // error handler right before selecting substructure redirection mask on the
  // root window, so invoked if and only if another window manager is running.
  static int onWmDetected(Display *display, XErrorEvent *e);
  // Whether an existing window manager was detected set by onWmDetected
  static bool wm_detected_;
};
