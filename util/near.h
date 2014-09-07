#ifndef NEAR_HPP_
#define NEAR_HPP_

#include <cmath>
#include <type_traits>

template <typename T>
inline typename std::enable_if<std::is_floating_point<T>::value, bool>::type
is_near(T v0, T v1, T eps) {
  return abs(v1 - v0) < eps;
}

#endif
