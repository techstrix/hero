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
template <typename T> struct Position {
  T x, y;
  Position() = default;

  Position(T _x, T _y) : x(_x), y(_y) {}

  std::string ToString() const;
};

template <typename T> struct Vector2D {
  T x, y;
  Vector2D() = default;

  Vector2D(T _x, T _y) : x(_x), y(_y) {}

  std::string ToString() const;
};

// Position operators
template <typename T>
std::ostream &operator<<(std::ostream &out, const Position<T> &pos);

template <typename T>
Vector2D<T> operator-(const Position<T> &a, const Position<T> &b);

template <typename T>
Position<T> operator+(const Position<T> &a, const Vector2D<T> &v);

template <typename T>
Position<T> operator+(const Vector2D<T> &v, const Position<T> &a);

template <typename T>
Position<T> operator-(const Position<T> &a, const Vector2D<T> &v);

// Size operators
template <typename T>
std::ostream &operator<<(std::ostream &out, const Size<T> &size);
template <typename T> Vector2D<T> operator-(const Size<T> &a, const Size<T> &b);

template <typename T> Size<T> operator+(const Size<T> &a, const Vector2D<T> &v);

template <typename T> Size<T> operator+(const Vector2D<T> &v, const Size<T> &a);

template <typename T> Size<T> operator-(const Size<T> &a, const Vector2D<T> &v);

// Implementation
template <typename T> std::string Size<T>::ToString() const {
  std::ostringstream out;

  out << width << "x" << height;
  return out.str();
}
template <typename T> std::string Position<T>::ToString() const {
  std::ostringstream out;
  out << "(" << x << ", " << y << ")";
  return out.str();
}
template <typename T>
std::ostream &operator<<(std::ostream &out, const Size<T> &size) {
  return out << size.ToString();
}
template <typename T>
std::ostream &operator<<(std::ostream &out, const Position<T> &pos) {
  return out << pos.ToString();
}

template <typename T>
Vector2D<T> operator-(const Position<T> &a, const Position<T> &b) {
  return Vector2D<T>(a.x - b.x, a.y - b.y);
}
template <typename T>
Position<T> operator+(const Position<T> &a, const Vector2D<T> &v) {
  return Position<T>(a.x + v.x, a.y + v.y);
}

template <typename T>
Position<T> operator+(const Vector2D<T> &v, const Position<T> &a) {
  return Position<T>(a.x + v.x, a.y + v.y);
}

template <typename T>
Position<T> operator-(const Position<T> &a, const Vector2D<T> &v) {
  return Position<T>(a.x - v.x, a.y - v.y);
}

template <typename T>
Vector2D<T> operator-(const Size<T> &a, const Size<T> &b) {
  return Vector2D<T>(a.x - b.x, a.y - b.y);
}
template <typename T>
Size<T> operator+(const Size<T> &a, const Vector2D<T> &v) {
  return Size<T>(a.x + v.x, a.y + v.y);
}

template <typename T>
Size<T> operator+(const Vector2D<T> &v, const Size<T> &a) {
  return Size<T>(a.x + v.x, a.y + v.y);
}

template <typename T>
Size<T> operator-(const Size<T> &a, const Vector2D<T> &v) {
  return Size<T>(a.x - v.x, a.y - v.y);
}

} // namespace window_utils
