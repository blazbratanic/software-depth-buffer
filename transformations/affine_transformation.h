#ifndef BILINEAR_INTERPOLATION_H_
#define BILINEAR_INTERPOLATION_H_

#include <cmath>
#include <stdlib.h>
#include <util/array2dview.h>
#include <util/clamp.h>
#include <util/fixed_size_matrix.hpp>
#include <transformations/coordinate_axes.h>
#include <transformations/transformation_matrix.h>
#include <transformations/conversions.h>

#if defined(__GCC__)
#define __SSE__
#define __forceinline inline __attribute__((always_inline))
#endif
#if defined(_MSC_VER) && (_MSC_VER >= 1700)
#define __SSE__
#endif
#if defined(__MINGW32__)
#define __SSE__
#endif

#ifdef __SSE__
#include <immintrin.h>
#include <util/sse_util.h>
#include <util/position.hpp>
#endif

namespace sil {

template <typename T, typename U>
void affine_transformation(ImPos<T> const &src, ImPos<T> &dst,
                           sil::TransformationMatrix<U> const &H) {
  dst.row = src.row * H.a + src.col * H.b + H.c;
  dst.col = src.row * H.d + src.col * H.e + H.f;
}

template <typename T, typename U>
ImPos<T> affine_transformation(ImPos<T> src,
                               sil::TransformationMatrix<U> const &H) {
  ImPos<T> dst;
  affine_transformation<T>(src, dst, H);
  return dst;
}

template <typename T, typename U>
void affine_transformation(std::vector<ImPos<T>> const &src,
                           std::vector<ImPos<T>> &dst,
                           sil::TransformationMatrix<U> const &H) {
  auto src_it = src.cbegin();
  auto dst_it = dst.begin();
  for (; src_it != src.cend(); ++src_it, ++dst_it)
    affine_transformation(*src_it, *dst_it, H);
}

template <typename T, typename U>
std::vector<ImPos<T>> affine_transformation(
    std::vector<ImPos<T>> const &src, sil::TransformationMatrix<U> const &H) {
  auto dst = std::vector<ImPos<T>>(src.size());
  affine_transformation<T>(src, dst, H);
  return dst;
}

namespace transformations {

template <typename T, typename U = T>
struct AffineTransformation {
#define FZERO Set1(0.0f)
#define FONE Set1(1.0f)
#define IZERO Set1(0)
#define IONE Set1(1)

  AffineTransformation();
  void operator()(ConstArray2dView<T> &src_array, Array2dView<U> &dst_array,
                  Homography homography);
  void Transform_Scalar_(ConstArray2dView<T> &src_array,
                         Array2dView<U> &dst_array, Homography homography);
  void Transform_SSE_(ConstArray2dView<T> &src_array, Array2dView<U> &dst_array,
                      Homography homography);

 private:
  __forceinline void interpolate(const int x, const int y,
                                 Homography &homography,
                                 ConstArray2dView<T> &src_array,
                                 Array2dView<U> &dst_array);

#ifdef __SSE__
  __forceinline void interpolate_sse(const int x, const int y,
                                     Homography &homography,
                                     ConstArray2dView<T> &src_array,
                                     Array2dView<U> &dst_array);

  const int vec_size_;
  FVec min_vec_index_;
  FVec max_vec_x_index_;
  FVec max_vec_y_index_;
  FVec increasing_x_;
#endif

  int max_x_index_;
  int max_y_index_;
};

#ifdef __SSE__
template <typename T, typename U>
AffineTransformation<T, U>::AffineTransformation()
    : vec_size_(sizeof(FVec) / sizeof(float)) {
  float increasing_x[8] = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f};
  increasing_x_ = Load(increasing_x);
}
#else
template <typename T, typename U>
AffineTransformation<T, U>::AffineTransformation() {}
#endif

template <typename T, typename U>
void AffineTransformation<T, U>::operator()(ConstArray2dView<T> &src_array,
                                            Array2dView<U> &dst_array,
                                            Homography homography) {
  max_x_index_ = src_array.GetSize1();
  max_y_index_ = src_array.GetSize0();

#ifdef __SSE__
  Transform_SSE_(src_array, dst_array, homography);
#else
  Transform_Scalar_(src_array, dst_array, homography);
#endif
}

template <typename T, typename U>
void AffineTransformation<T, U>::Transform_Scalar_(
    ConstArray2dView<T> &src_array, Array2dView<U> &dst_array,
    Homography homography) {
  int rows = dst_array.GetSize0();
  int cols = dst_array.GetSize1();
  max_x_index_ = src_array.GetSize1();
  max_y_index_ = src_array.GetSize0();

  for (int y = 0; y < rows; y++)
    for (int x = 0; x < cols; x++)
      interpolate(x, y, homography, src_array, dst_array);
}

template <typename T, typename U>
__forceinline void AffineTransformation<T, U>::interpolate(
    const int x, const int y, Homography &homography,
    ConstArray2dView<T> &src_array, Array2dView<U> &dst_array) {
  // Homography
  float tX = homography.a * x + homography.b * y + homography.c;
  float tY = homography.d * x + homography.e * y + homography.f;

  // Repeat edge elements
  tX = clamp<float>(tX, 0, max_x_index_ - 2);
  tY = clamp<float>(tY, 0, max_y_index_ - 2);

  int iX = static_cast<int>(tX);
  int iY = static_cast<int>(tY);

  // Bilinear interpolation
  float kx0 = tX - iX;
  float ky0 = tY - iY;
  float kx1 = 1.0f - kx0;
  float ky1 = 1.0f - ky0;

  float interpolated_value = 0;
  interpolated_value += kx1 * ky1 * src_array(iY, iX);
  interpolated_value += kx0 * ky1 * src_array(iY, iX + 1);
  interpolated_value += kx1 * ky0 * src_array(iY + 1, iX);
  interpolated_value += kx0 * ky0 * src_array(iY + 1, iX + 1);
  dst_array(y, x) = convert<U>(interpolated_value);
}

#ifdef __SSE__
template <typename T, typename U>
void AffineTransformation<T, U>::Transform_SSE_(ConstArray2dView<T> &src_array,
                                                Array2dView<U> &dst_array,
                                                Homography homography) {
  int rows = dst_array.GetSize0();
  int cols = dst_array.GetSize1();

  max_x_index_ = src_array.GetSize1();
  max_y_index_ = src_array.GetSize0();
  min_vec_index_ = Set1(static_cast<float>(0.f));
  max_vec_x_index_ = Set1(static_cast<float>(max_x_index_) - 2.f);
  max_vec_y_index_ = Set1(static_cast<float>(max_y_index_) - 2.f);

  for (int y = 0; y < rows; y++) {
    std::pair<int, int> sse_range = dst_array.GetSSERange(y, vec_size_);

    for (int x = 0; x < sse_range.first; x++)
      interpolate(x, y, homography, src_array, dst_array);

    for (int x = sse_range.first; x < sse_range.second; x += vec_size_)
      interpolate_sse(x, y, homography, src_array, dst_array);

    for (int x = sse_range.second; x < cols; x++)
      interpolate(x, y, homography, src_array, dst_array);
  }
}

template <typename T, typename U>
__forceinline void AffineTransformation<T, U>::interpolate_sse(
    const int x, const int y, Homography &homography,
    ConstArray2dView<T> &src_array, Array2dView<U> &dst_array) {
  // x needs an increasing (0, 1,...n) addition, otherwise we would simply
  // repeat the same x
  FVec x_vec = Add(ToFloat(Set1(x)), increasing_x_);
  FVec y_vec = ToFloat(Set1(y));

  // Homography
  FVec tx_vec = Add3(Mul(Set1(homography.a), x_vec),
                     Mul(Set1(homography.b), y_vec), Set1(homography.c));

  FVec ty_vec = Add3(Mul(Set1(homography.d), x_vec),
                     Mul(Set1(homography.e), y_vec), Set1(homography.f));

  // Check if out of bounds
  tx_vec = Min(max_vec_x_index_, Max(min_vec_index_, tx_vec));
  ty_vec = Min(max_vec_y_index_, Max(min_vec_index_, ty_vec));

  // Scalar and float methods seem to give different results if I don't use
  // Floor.
  IVec cols_vec = Set1(max_x_index_);
  FVec iX = Floor(tx_vec);
  FVec iY = Floor(ty_vec);
  IVec target_vec =
      ToInt(Add(iX, Mul(iY, Set1(static_cast<float>(max_x_index_)))));

  // Bilinear interpolation
  IVec i00 = target_vec;
  IVec i01 = Add(target_vec, IONE);
  IVec i10 = Add(target_vec, cols_vec);
  IVec i11 = Add3(target_vec, cols_vec, IONE);

  FVec kx0 = Sub(tx_vec, iX);
  FVec ky0 = Sub(ty_vec, iY);
  FVec kx1 = Sub(FONE, kx0);
  FVec ky1 = Sub(FONE, ky0);

  FVec interpolated_values = FONE;
  interpolated_values = Add(Add(Mul3(kx1, ky1, Gather(src_array[0], i00)),
                                Mul3(kx0, ky1, Gather(src_array[0], i01))),
                            Add(Mul3(kx1, ky0, Gather(src_array[0], i10)),
                                Mul3(kx0, ky0, Gather(src_array[0], i11))));

  Store(&dst_array(y, x), interpolated_values);
}
#endif
}

template <typename T, typename U>
Array2dView<U> affine_transformation(ConstArray2dView<T> src,
                                     Array2dView<U> dst, Homography H) {
  transformations::AffineTransformation<T, U> affine_transformer;
  affine_transformer(src, dst, H);
  return dst;
}

template <typename T, typename U>
Array2dView<U> rotate_array(ConstArray2dView<T> src, Array2dView<U> dst,
                            double angle, ImPos<float> anchor_point) {

  auto H =
      TransformationMatrix<float>::template Rotate<float>(angle, anchor_point);
  affine_transformation(src, dst, H);
  return dst;
}
}

#endif
