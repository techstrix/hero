#pragma once
#include <X11/Xlib.h>
#include <ostream>
#include <sstream>
#include <string>

namespace window_utils {
std::string ToString(const XEvent &e);
template <typename T> struct Size {
  T width, height;
  Size() = default;

  Size(T w, T h) : width(w), height(h) {}
  std::string ToString() const;
};

template <typename T>
std::ostream &operator<<(std::ostream &out, const Size<T> &size);

// Implementation
template <typename T> std::string Size<T>::ToString() const {
  std::ostringstream out;

  out << width << "x" << height;
  return out.str();
}
template <typename T>
std::ostream &operator<<(std::ostream &out, const Size<T> &size) {
  return out << size.ToString();
}

} // namespace window_utils
