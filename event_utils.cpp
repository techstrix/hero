#include "event_utils.hpp"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <string>

namespace window_utils {
std::string ToString(const XEvent &e) {
  switch (e.type) {
  case CreateNotify:
    return "CreateNotify";
  case DestroyNotify:
    return "DestroyNotify";

  case MapNotify:
    return "MapNotify";

  case ConfigureRequest:
    return "ConfigureRequest";

  case MapRequest:
    return "MapRequest";

  default:
    return "Unknown event (" + std::to_string(e.type) + ")";
  }
}

} // namespace window_utils
