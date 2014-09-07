#ifndef COORDINATE_AXES_H
#define COORDINATE_AXES_H

#include <utility>
#include <util/position.hpp>

namespace sil {

template <typename T = int>
struct CoordinateAxes {
  static CoordinateAxes Make(ImPos<T> origin_, float orientation_) {
    CoordinateAxes<T> ca;
    ca.origin = origin_;
    ca.orientation = orientation_;
    return ca;
  }

  ImPos<T> origin;
  float orientation;
};
}  // sil
#endif
