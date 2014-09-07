/*
 * =====================================================================================
 *
 *       Filename:  fastatan2.h
 *
 *    Description:  Fast arctan2 implementation, based on atan2 approximation
 *                  maximum error < .01 rad
 *
 *        Version:  1.0
 *        Created:  12/16/12 00:22:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Blaz Bratanic
 *   Organization:
 *
 * =====================================================================================
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace constants {
template <typename T>
T pi() {
  return static_cast<T>(3.1415926535897932384626433832795e0);
}
template <typename T>
T two_pi() {
  return static_cast<T>(6.283185307179586476925286766559e0);
}
template <typename T>
T half_pi() {
  return static_cast<T>(1.5707963267948966192313216916398e0);
}
template <typename T>
T quarter_pi() {
  return static_cast<T>(7.8539816339744830961566084581988e-1);
}
template <typename T>
T reciprocal_pi() {
  return static_cast<T>(1.0 / 3.1415926535897932384626433832795e0);
}
template <typename T>
T reciprocal_two_pi() {
  return static_cast<T>(1.0 / 6.283185307179586476925286766559e0);
}
template <typename T>
T reciprocal_half_pi() {
  return static_cast<T>(1.0 / 1.5707963267948966192313216916398e0);
}
template <typename T>
T reciprocal_quarter_pi() {
  return static_cast<T>(1.0 / 7.8539816339744830961566084581988e-1);
}
template <typename T>
T deg2rad() {
  return static_cast<T>(1.7453292519943295769236907684886e-2);
}
template <typename T>
T rad2deg() {
  return static_cast<T>(5.7295779513082320876798154814105e1);
}
template <typename T>
T e() {
  return static_cast<T>(2.7182818284590452353602874713527e0);
}
template <typename T>
T sqrt2() {
  return static_cast<T>(1.4142135623730950488016887242097e0);
}
template <typename T>
T inv_sqrt2() {
  return static_cast<T>(7.0710678118654752440084436210485e-1);
}
template <typename T>
T sqrt3() {
  return static_cast<T>(1.7320508075688772935274463415059e0);
}
}
#endif
