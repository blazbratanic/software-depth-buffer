#ifndef WRAP_ANGLES_H
#define WRAP_ANGLES_H

#include <util/constants.h>

namespace detail {
template <typename T>
T wrap(T value, T high, T reciprocal_high) {
  value += 100 * constants::two_pi<T>();
  return value - high * int(value * reciprocal_high);
}
}

template <typename T>
T wrap_half_pi(T value) {
  return detail::wrap(value, constants::half_pi<T>(),
                      constants::reciprocal_half_pi<T>());
}

template <typename T>
T wrap_pi(T value) {
  return detail::wrap(value, constants::pi<T>(), constants::reciprocal_pi<T>());
}

template <typename T>
T wrap_two_pi(T value) {
  return detail::wrap(value, constants::two_pi<T>(),
                      constants::reciprocal_two_pi<T>());
}

template <typename T>
T wrap_minus_half_pi_to_half_pi(T value) {
  return wrap_pi(value + constants::half_pi<T>()) - constants::half_pi<T>();
}

template <typename T>
T wrap_minus_pi_to_pi(T value) {
  return wrap_two_pi(value + constants::pi<T>()) - constants::pi<T>();
}

#endif
