#ifndef POSITION_HPP
#define POSITION_HPP
#include <algorithm>

template <typename T>
struct ImPos {
  T row;
  T col;
};

template <typename T, typename U>
struct OrientedImPos {
  ImPos<T> position;
  U orientation;
};

template <typename T>
ImPos<T> operator+(ImPos<T> const& a, ImPos<T> const& b) {
  return {a.row + b.row, a.col + b.col};
}
template <typename T>
ImPos<T> operator-(ImPos<T> const& a, ImPos<T> const& b) {
  return {a.row - b.row, a.col - b.col};
}
template <typename T>
ImPos<T> operator-(ImPos<T> const& a) {
  return {-a.row, -a.col};
}

template <typename T, typename U>
ImPos<T> operator/(ImPos<T> const& a, U b) {
  return {a.row / b, a.col / b};
}

template <typename T, typename U>
ImPos<T> operator*(ImPos<T> const& a, U b) {
  return {a.row * b, a.col * b};
}

template <typename T>
struct Boundary {
  ImPos<T> lu;
  ImPos<T> rb;
};

template <typename T>
ImPos<T> min(ImPos<T> const& a, ImPos<T> const& b) {
  return {std::min(a.row, b.row), std::min(a.col, b.col)};
}

template <typename T>
ImPos<T> max(ImPos<T> const& a, ImPos<T> const& b) {
  return {std::max(a.row, b.row), std::max(a.col, b.col)};
}

#endif
