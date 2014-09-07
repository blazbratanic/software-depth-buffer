#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <util/array2dview.h>
#include <util/clamp.h>
#include <transformations/conversions.h>

namespace sil {
namespace transformations {

template <typename T, typename U>
T bilinear_interpolation(U tR, U tC, ConstArray2dView<T> const &src) {
  // Repeat edge elements
  tR = clamp<U>(tR, 0, src.GetSize0() - 2);
  tC = clamp<U>(tC, 0, src.GetSize1() - 2);

  int iC = static_cast<int>(tC);
  int iR = static_cast<int>(tR);

  // Bilinear interpolation
  float kx0 = tC - iC;
  float ky0 = tR - iR;
  float kx1 = 1.0f - kx0;
  float ky1 = 1.0f - ky0;

  float interpolated_value = 0;
  interpolated_value += kx1 * ky1 * src(iR, iC);
  interpolated_value += kx0 * ky1 * src(iR, iC + 1);
  interpolated_value += kx1 * ky0 * src(iR + 1, iC);
  interpolated_value += kx0 * ky0 * src(iR + 1, iC + 1);
  return convert<T>(interpolated_value);
}

template <typename T, typename U, typename V>
Array2dView<V> bilinear_interpolation(ConstArray2dView<T> &src,
                                      ConstArray2dView<U> z_row,
                                      ConstArray2dView<U> z_col,
                                      Array2dView<V> &dst) {

  int rows = dst.GetSize0();
  int cols = dst.GetSize1();

  for (int r = 0; r < rows; r++) {
    auto zr_it = z_row.Begin1(r);
    auto zc_it = z_col.Begin1(r);
    auto dst_it = dst.Begin1(r);

    for (int c = 0; c < cols; c++)
      *dst_it++ = interpolate(*zr_it++, *zc_it++, src);
  }
  return dst;
}
}  // transformations

template <typename T, typename U, typename V>
Array2dView<V> bilinear_interpolation(ConstArray2dView<T> &src_array,
                                      ConstArray2dView<U> z_row,
                                      ConstArray2dView<U> z_col,
                                      Array2dView<V> &dst_array) {
  return transformations::bilinear_interpolation(src_array, z_row, z_col,
                                                 dst_array);
}
}  // sil

#endif
