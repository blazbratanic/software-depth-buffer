#ifndef CLAMP_H
#define CLAMP_H

#include <algorithm>

template <typename T>
T clamp(T value, T low, T high) {
  return std::min(std::max(value, low), high);
}

#endif
