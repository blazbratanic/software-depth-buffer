#ifndef TRANSFORMATIONS_CONVERSIONS_H
#define TRANSFORMATIONS_CONVERSIONS_H

#include <cmath>

namespace sil {
namespace transformations {
template <typename T>
static inline T convert(float a) {
  return std::floor(a + .5);
}

template <>
inline float convert<float>(float a) {
  return a;
}

template <>
inline double convert<double>(float a) {
  return a;
}
}
}

#endif
