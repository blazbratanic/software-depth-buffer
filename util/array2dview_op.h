#ifndef ARRAY2DVIEW_OP_H_
#define ARRAY2DVIEW_OP_H_

#include <util/array2dview.h>
#include <util/array2d.h>
#include <util/sse_util.h>
#include <util/fast_math.h>
#include <util/functors.h>
//#include <cstddef>

#include <functional>
#include <cmath>

namespace sil {

template <typename T, typename U, typename UnaryOperator>
Array2dView<U> transform(ConstArray2dView<T> src, Array2dView<U> dst,
                         UnaryOperator op) {
  if (!same_size(src, dst))
    throw std::runtime_error("Incompatible input and output array sizes.");

  for (size_t i0 = 0; i0 < src.GetSize0(); i0++) {
    const T* src_row = &src(i0, 0);
    U* dst_row = &dst(i0, 0);

    for (size_t i1 = 0; i1 < src.GetSize1(); i1++) {
      *dst_row = op(*src_row);
      ++dst_row;
      ++src_row;
    }
  }
  return dst;
}

template <typename T, typename U, typename V, typename BinaryOperator>
Array2dView<V> transform(ConstArray2dView<T> src1, ConstArray2dView<U> src2,
                         Array2dView<V> dst, BinaryOperator op) {
  if (!(same_size(src1, dst) && same_size(src2, dst)))
    throw std::runtime_error("Incompatible input and output array sizes.");

  for (size_t i0 = 0; i0 < src1.GetSize0(); i0++) {
    const T* src1_row = &src1(i0, 0);
    const U* src2_row = &src2(i0, 0);
    V* dst_row = &dst(i0, 0);

    for (size_t i1 = 0; i1 < src1.GetSize1(); i1++) {
      *dst_row = op(*src1_row, *src2_row);
      ++dst_row;
      ++src1_row;
      ++src2_row;
    }
  }
  return dst;
}

template <typename T, typename U, typename V, typename Z,
          typename TernaryOperator>
Array2dView<Z> transform(ConstArray2dView<T> src1, ConstArray2dView<U> src2,
                         ConstArray2dView<V> src3, Array2dView<Z> dst,
                         TernaryOperator op) {
  if (!(same_size(src1, dst) && same_size(src2, dst) && same_size(src3, dst)))
    throw std::runtime_error("Incompatible input and output array sizes.");

  for (size_t i0 = 0; i0 < src1.GetSize0(); i0++) {
    const T* src1_row = &src1(i0, 0);
    const U* src2_row = &src2(i0, 0);
    const V* src3_row = &src3(i0, 0);
    Z* dst_row = &dst(i0, 0);

    for (size_t i1 = 0; i1 < src1.GetSize1(); i1++) {
      *dst_row = op(*src1_row, *src2_row, *src3_row);
      ++dst_row;
      ++src1_row;
      ++src2_row;
      ++src3_row;
    }
  }
  return dst;
}

template <typename T, typename U>
void copy(ConstArray2dView<T> src, Array2dView<U> dst) {
  sil::transform(src, dst, sil::functors::unary_identity<T>());
}

template <typename T, typename U>
void copy_rows(ConstArray2dView<T> src, Array2dView<U> dst, size_t first,
               size_t last) {
  ConstArray2dView<T> s_(src, first, 0, last, src.GetSize1());
  Array2dView<U> d_(dst, first, 0, last, dst.GetSize1());
  sil::copy(s_, d_);
}

template <typename T, typename U>
void copy_first_n_rows(ConstArray2dView<T> src, Array2dView<U> dst, size_t n) {
  copy_rows(src, dst, 0, n);
}

template <typename T, typename U>
void copy_first_row(ConstArray2dView<T> src, Array2dView<U> dst) {
  copy_first_n_rows(src, dst, 1);
}

template <typename T, typename U>
void copy_last_n_rows(ConstArray2dView<T> src, Array2dView<U> dst, size_t n) {
  copy_rows(src, dst, src.GetSize0() - n, src.GetSize0());
}

template <typename T, typename U>
void copy_last_row(ConstArray2dView<T> src, Array2dView<U> dst) {
  copy_last_n_rows(src, dst, 1);
}

template <typename T, typename U>
void copy_cols(ConstArray2dView<T> src, Array2dView<U> dst, size_t first,
               size_t last) {
  ConstArray2dView<T> s_(src, 0, first, src.GetSize0(), last);
  Array2dView<U> d_(dst, 0, first, src.GetSize0(), last);
  sil::copy(s_, d_);
}

template <typename T, typename U>
void copy_first_n_cols(ConstArray2dView<T> src, Array2dView<U> dst, size_t n) {
  copy_cols(src, dst, 0, n);
}

template <typename T, typename U>
void copy_first_col(ConstArray2dView<T> src, Array2dView<U> dst) {
  copy_first_n_cols(src, dst, 1);
}

template <typename T, typename U>
void copy_last_n_cols(ConstArray2dView<T> src, Array2dView<U> dst, size_t n) {
  copy_cols(src, dst, src.GetSize1() - n, src.GetSize1());
}

template <typename T, typename U>
void copy_last_col(ConstArray2dView<T> src, Array2dView<U> dst) {
  copy_last_n_cols(src, dst, 1);
}

namespace detail {
template <typename T>
struct array_op {
  static const bool value = false;
};
template <typename T>
struct array_op<Array2d<T>> {
  static const bool value = true;
};
template <typename T>
struct array_op<Array2dView<T>> {
  static const bool value = true;
};
template <typename T>
struct array_op<ConstArray2dView<T>> {
  static const bool value = true;
};

struct yes_type {
  char a;
};
struct no_type {
  char b[2];
};

template <typename T,
          typename = typename std::enable_if<array_op<T>::value>::type>
yes_type contains_array_helper(T);
template <typename T>
no_type contains_array_helper(...);

template <typename T>
struct contains_array {
  static const bool value =
      sizeof(contains_array_helper<T>(T())) == sizeof(yes_type);
};
}

#define ARRAY2DVIEW_ELEMENTARY_OP_SCALAR(name, op)                             \
  template <typename T, typename U, typename V,                                \
            typename = typename std::enable_if<                                \
                !detail::contains_array<U>::value>::type>                      \
  Array2dView<V> name(ConstArray2dView<T> src, U factor, Array2dView<V> dst) { \
    sil::transform(src, dst, std::bind1st(op<T>(), factor));                   \
    return dst;                                                                \
  }

#define ARRAY2DVIEW_ELEMENTARY_OP_ARRAY(name, op)                         \
  template <typename T, typename U, typename V>                           \
  Array2dView<V> name(ConstArray2dView<T> src1, ConstArray2dView<U> src2, \
                      Array2dView<V> dst) {                               \
    sil::transform(src1, src2, dst, op<T>());                             \
    return dst;                                                           \
  }

ARRAY2DVIEW_ELEMENTARY_OP_SCALAR(multiply, std::multiplies)
ARRAY2DVIEW_ELEMENTARY_OP_ARRAY(multiply, std::multiplies)

ARRAY2DVIEW_ELEMENTARY_OP_SCALAR(divide, std::divides)
ARRAY2DVIEW_ELEMENTARY_OP_ARRAY(divide, std::divides)

ARRAY2DVIEW_ELEMENTARY_OP_SCALAR(minus, std::minus)
ARRAY2DVIEW_ELEMENTARY_OP_ARRAY(minus, std::minus)

ARRAY2DVIEW_ELEMENTARY_OP_SCALAR(plus, std::plus)
ARRAY2DVIEW_ELEMENTARY_OP_ARRAY(plus, std::plus)

#undef ARRAY2DVIEW_ELEMENTARY_OP_SCALAR
#undef ARRAY2DVIEW_ELEMENTARY_OP_ARRAY

template <typename T>
void fill(Array2dView<T> array, T value) {
  sil::transform(array, array, sil::functors::constant<T>(value));
}

template <typename T, typename U>
void conditional_fill(Array2dView<T> array, ConstArray2dView<U> condition,
                      T value) {
  sil::transform(array, condition, array,
                 sil::functors::conditional_constant<T, U>(value));
}

template <typename T, typename U>
bool is_same_size(ConstArray2dView<T> a, ConstArray2dView<U> b) {
  return (a.GetSize0() == b.GetSize0()) && (a.GetSize1() == b.GetSize1());
}

template <typename T>
void fill_borders(Array2dView<T> src, T value = T(),
                  size_t nborder_elements = 1) {
  sil::fill(get_last_n_cols(src, nborder_elements), value);
  sil::fill(get_last_n_rows(src, nborder_elements), value);
  sil::fill(get_first_n_cols(src, nborder_elements), value);
  sil::fill(get_first_n_rows(src, nborder_elements), value);
}

template <typename T>
void zero_borders(Array2dView<T> src, size_t nborder_elements = 1) {
  fill_borders(src, T(0), nborder_elements);
}

template <typename T>
void enforce_lower_bound(Array2dView<T> array, T minimum_value) {
  sil::transform(array, array, sil::functors::lower_bound<T>(minimum_value));
}

template <typename T>
void enforce_upper_bound(Array2dView<T> array, T maximum_value) {
  sil::transform(array, array, sil::functors::upper_bound<T>(maximum_value));
}

template <typename T>
const T* max_element(ConstArray2dView<T> src) {
  T max_el = std::numeric_limits<T>::lowest();
  const T* max_it = &src(0, 0);

  for (size_t i0 = 0; i0 < src.rows(); i0++) {
    const T* row_it = &src(i0, 0);
    for (size_t i1 = 0; i1 < src.cols(); i1++, row_it++) {
      if (*row_it > max_el) {
        max_el = *row_it;
        max_it = row_it;
      }
    }
  }
  return max_it;
}

template <typename T>
const T* min_element(ConstArray2dView<T> src) {
  T min_el = std::numeric_limits<T>::max();
  const T* min_it = &src(0, 0);

  for (size_t i0 = 0; i0 < src.rows(); i0++) {
    const T* row_it = &src(i0, 0);
    for (size_t i1 = 0; i1 < src.cols(); i1++, row_it++) {
      if (*row_it < min_el) {
        min_el = *row_it;
        min_it = row_it;
      }
    }
  }
  return min_it;
}


#if defined __SSE__ || defined __AVX__
#include <xmmintrin.h>
#else
#define __forceinline
#endif

template <int simdSize, class S1, class S2, class D, class Fun, class SIMDFun>
__forceinline void simd_transform_1d(ConstArray2dView<S1> src1,
                                     ConstArray2dView<S2> src2,
                                     Array2dView<D> dst, Fun fun,
                                     SIMDFun simdFun) {

  std::pair<size_t, size_t> limits = dst.GetSSERange(0, simdSize);
  // printf("ptr %p, nx %d, start %d, end %d\n", dst.GetData(), dst.GetSize1(),
  // limits.first, limits.second);
  const S1* s1 = src1.GetData();
  const S2* s2 = src2.GetData();
  D* d = dst.GetData();
  for (size_t i = 0; i < limits.first; i++) fun(s1 + i, s2 + i, d + i);

  for (size_t i = limits.first; i < limits.second; i += simdSize)
    simdFun(s1 + i, s2 + i, d + i);

  for (size_t i = limits.second; i < dst.GetSize1(); i++)
    fun(s1 + i, s2 + i, d + i);
}

template <int simdSize, class S1, class S2, class D, class Fun, class SIMDFun>
void simd_transform(ConstArray2dView<S1> src1, ConstArray2dView<S2> src2,
                    Array2dView<D> dst, Fun fun, SIMDFun simdFun) {
  assert(same_size(src1, dst));
  assert(same_size(src2, dst));
  size_t rows = dst.GetSize0();
  size_t cols = dst.GetSize1();

  if (rows > 1 && src1.IsContiguous() && src2.IsContiguous() &&
      dst.IsContiguous())
    simd_transform_1d<simdSize>(make_const_view(src1.GetData(), 1, rows * cols),
                                make_const_view(src2.GetData(), 1, rows * cols),
                                make_view(dst.GetData(), 1, rows * cols), fun,
                                simdFun);
  else
    for (size_t i0 = 0; i0 < dst.GetSize0(); i0++)
      simd_transform_1d<simdSize>(
          src1.SubView(i0, 0, i0 + 1, cols), src2.SubView(i0, 0, i0 + 1, cols),
          dst.SubView(i0, 0, i0 + 1, cols), fun, simdFun);
}

template <int simdSize, class S1, class S2, class S3, class D, class Fun,
          class SIMDFun>
__forceinline void simd_transform_1d(ConstArray2dView<S1> src1,
                                     ConstArray2dView<S2> src2,
                                     ConstArray2dView<S3> src3,
                                     Array2dView<D> dst, Fun fun,
                                     SIMDFun simdFun) {
  assert(same_size(src1, dst));
  assert(same_size(src2, dst));
  assert(same_size(src3, dst));
  assert(dst.GetSize0() == 1);

  std::pair<size_t, size_t> limits = dst.GetSSERange(0, simdSize);

  const S1* s1 = src1.GetData();
  const S2* s2 = src2.GetData();
  const S3* s3 = src3.GetData();
  D* d = dst.GetData();
  for (size_t i = 0; i < limits.first; i++)
    fun(s1 + i, s2 + i, s3 + i, d + i);

  for (size_t i = limits.first; i < limits.second; i += simdSize)
    simdFun(s1 + i, s2 + i, s3 + i, d + i);

  for (size_t i = limits.second; i < dst.GetSize1(); i++)
    fun(s1 + i, s2 + i, s3 + i, d + i);
}

template <int simdSize, class S1, class S2, class S3, class D, class Fun,
          class SIMDFun>
void simd_transform(ConstArray2dView<S1> src1, ConstArray2dView<S2> src2,
                    ConstArray2dView<S3> src3, Array2dView<D> dst, Fun fun,
                    SIMDFun simdFun) {

  assert(same_size(src1, dst));
  assert(same_size(src2, dst));
  assert(same_size(src3, dst));

  size_t rows = dst.GetSize0();
  size_t cols = dst.GetSize0();

  if (rows > 1 && src1.IsContiguous() && src2.IsContiguous() &&
      dst.IsContiguous())
    simd_transform_1d<simdSize>(make_const_view(src1.GetData(), 1, rows * cols),
                                make_const_view(src2.GetData(), 1, rows * cols),
                                make_const_view(src3.GetData(), 1, rows * cols),
                                make_view(dst.GetData(), 1, rows * cols), fun,
                                simdFun);
  else
    for (size_t i0 = 0; i0 < dst.GetSize0(); i0++)
      simd_transform_1d<simdSize>(
          src1.SubView(i0, 0, i0 + 1, cols), src2.SubView(i0, 0, i0 + 1, cols),
          src3.SubView(i0, 0, i0 + 1, cols), dst.SubView(i0, 0, i0 + 1, cols),
          fun, simdFun);
}

namespace simd_functors {

#define BINARY_OP_FUNCTORS(name, op)                        \
  template <typename T>                                     \
  struct name {                                             \
    void operator()(const T* src1, const T* src2, T* dst) { \
      *dst = *src1 op * src2;                               \
    }                                                       \
  };

BINARY_OP_FUNCTORS(add, +)
BINARY_OP_FUNCTORS(sub, -)
BINARY_OP_FUNCTORS(mul, *)
BINARY_OP_FUNCTORS(bitwise_or, | )
#undef BINARY_OP_FUNCTORS

#define BINARY_OP_FUNCTORS_SSE(name, op)                                \
  struct name##_sse {                                                   \
    void operator()(const float* src1, const float* src2, float* dst) { \
      Store(dst, _mm_##name##_ps(Load(src1), Load(src2)));              \
    }                                                                   \
  };

BINARY_OP_FUNCTORS_SSE(add, +)
BINARY_OP_FUNCTORS_SSE(sub, -)
BINARY_OP_FUNCTORS_SSE(mul, *)
#undef BINARY_OP_FUNCTORS_SSE
struct bitwise_or_sse {
  void operator()(const int* src1, const int* src2, int* dst) {
    _mm_store_si128((__m128i*)dst,
                    _mm_or_si128(_mm_loadu_si128((__m128i*)src1),
                                 _mm_loadu_si128((__m128i*)src2)));
  }
};
}

#define BINARY_1D_OP(name)                                                    \
  template <typename T>                                                       \
  __forceinline void name##_sse(ConstArray2dView<T> src1,                     \
                                ConstArray2dView<T> src2,                     \
                                Array2dView<T> dst) {                         \
    simd_transform<16 / sizeof(T)>(src1, src2, dst, simd_functors::name<T>(), \
                                   simd_functors::name##_sse());              \
  }

BINARY_1D_OP(add)
BINARY_1D_OP(sub)
BINARY_1D_OP(mul)
BINARY_1D_OP(bitwise_or)

#undef BINARY_1D_OP

}  // sil
#endif
