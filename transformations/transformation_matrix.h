#ifndef TRANSFORMATION_MATRIX_H
#define TRANSFORMATION_MATRIX_H

#include <transformations/coordinate_axes.h>
#include <util/position.hpp>
#include <cmath>
#include <iostream>

namespace sil {
//
//  | a  b  c |
//  | d  e  f |
//  | g  h  i |
//
template <typename T>
struct TransformationMatrix {
  static TransformationMatrix Make(T a_ = 1, T b_ = 0, T c_ = 0, T d_ = 0,
                                   T e_ = 1, T f_ = 0, T g_ = 0, T h_ = 0,
                                   T i_ = 1) {
    TransformationMatrix t;
    t.a = a_;
    t.b = b_;
    t.c = c_;
    t.d = d_;
    t.e = e_;
    t.f = f_;
    t.g = g_;
    t.h = h_;
    t.i = i_;
    return t;
  }

  /*
  % Calculates transformation matrix from one coordinate axes to another
  syms src_x src_y src_angle dst_x dst_y dst_angle
  T0 = [1, 0, -src_x; 0, 1, -src_y; 0, 0, 1];
  %angle_diff = dst_angle - src_angle;
  Ta = [cos(angle_diff), -sin(angle_diff), 0; sin(angle_diff), cos(angle_diff),
  0; 0, 0, 1];
  T1 = [ 1, 0, dst_x; 0, 1, dst_y; 0, 0, 1];

  H = T1 * Ta * T0

  H =
      [ cos(angle_diff), -sin(angle_diff), dst_x - src_x*cos(angle_diff) +
  src_y*sin(angle_diff)]
      [ sin(angle_diff),  cos(angle_diff), dst_y - src_y*cos(angle_diff) -
  src_x*sin(angle_diff)]
      [               0,                0, 1]
  */

  template <typename U>
  static TransformationMatrix MakeFwd(CoordinateAxes<U> src,
                                      CoordinateAxes<U> dst) {
    return TransformationMatrix::Make(src, dst);
  }

  template <typename U>
  static TransformationMatrix MakeBck(CoordinateAxes<U> src,
                                      CoordinateAxes<U> dst) {
    return TransformationMatrix::Make(dst, src);
  }

  template <typename U>
  static TransformationMatrix Make(CoordinateAxes<U> src,
                                   CoordinateAxes<U> dst) {

    TransformationMatrix t;

    double src_x = src.origin.col;
    double src_y = src.origin.row;
    double dst_x = dst.origin.col;
    double dst_y = dst.origin.row;
    double angle_diff = dst.orientation - src.orientation;

    t.a = cos(angle_diff);
    t.b = -sin(angle_diff);
    t.c = dst_x - src_x * cos(angle_diff) + src_y * sin(angle_diff);
    t.d = sin(angle_diff);
    t.e = cos(angle_diff);
    t.f = dst_y - src_y * cos(angle_diff) - src_x * sin(angle_diff);
    t.g = 0;
    t.h = 0;
    t.i = 1;

    return t;
  }

  template <typename U>
  static TransformationMatrix Rotate(double phi,
                                     ImPos<U> rotation_center = {U(0), U(0)}) {
    auto origin = sil::CoordinateAxes<U>::Make(rotation_center, 0);
    auto target = sil::CoordinateAxes<U>::Make(rotation_center, phi);
    return TransformationMatrix::MakeBck(origin, target);
  }

  static TransformationMatrix Scale(double scale) {
    return TransformationMatrix::Make(1.0 / scale, 0, 0, 0, 1.0 / scale, 0);
  }

  T a;
  T b;
  T c;
  T d;
  T e;
  T f;
  T g;
  T h;
  T i;
};

template <typename T>
std::ostream& operator<<(std::ostream& out, TransformationMatrix<T> const& H) {
  out << H.a << " " << H.b << " " << H.c << std::endl << H.d << " " << H.e
      << " " << H.f << std::endl << H.g << " " << H.h << " " << H.i
      << std::endl;
  return out;
}

typedef TransformationMatrix<float> Homography;

}  // sil
#endif
