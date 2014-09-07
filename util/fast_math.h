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
#ifndef fast_math_h
#define fast_math_h

#include <math.h>
#include <util/constants.h>

#define real_t float

// |error| < 0.005
#ifdef APPROX_ATAN
inline real_t fast_atan2f(real_t y, real_t x) {
  if (x == 0.0) {
    if (y > 0.0) return constants::half_pi<real_t>();
    if (y == 0.0) return 0.0;
    return -constants::half_pi<real_t>();
  }
  real_t atan;
  real_t z = y / x;
  if (fabsf(z) < 1.0) {
    atan = z / (1.0 + 0.28 * z * z);
    if (x < 0.0) {
      if (y < 0.0) return atan - constants::pi<real_t>();
      return atan + constants::pi<real_t>();
    }
  } else {
    atan = constants::half_pi<real_t>() - z / (z * z + 0.28);
    if (y < 0.0) return atan - constants::pi<real_t>();
  }
  return atan;
}

inline real_t fast_atanf(real_t y, real_t x) {
  if (x == 0.0) {
    if (y > 0.0) return constants::half_pi<real_t>();
    if (y == 0.0) return 0.0;
    return -constants::half_pi<real_t>();
  }
  real_t atan;
  real_t z = y / x;
  if (fabsf(z) < 1.0) {
    atan = z / (1.0 + 0.28 * z * z);
    return atan;
  } else {
    atan = constants::half_pi<real_t>() - z / (z * z + 0.28);
    if (x * y < 0.0) {
      return atan - constants::pi<real_t>();
    }
    //	if ( y < 0.0f ) return atan - PI;
  }
  return atan;
}

#else
inline real_t fast_atan2f(real_t y, real_t x) {
  if (x == 0.0) {
    if (y > 0.0) return constants::half_pi<real_t>();
    if (y == 0.0) return 0.0;
    return -constants::half_pi<real_t>();
  }
  return atan2(y, x);
}

inline real_t fast_atanf(real_t y, real_t x) {
  if (x == 0.0) {
    if (y > 0.0) return constants::half_pi<real_t>();
    if (y == 0.0) return 0.0;
    return -constants::half_pi<real_t>();
  }
  return atan(y / x);
}

#endif

#undef real_t

#endif
