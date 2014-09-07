#include <cmath>

#if defined(__SSE__) || defined(__AVX__) || defined(__SSE4__)
#include <immintrin.h>
#else
#define SSEUTIL_H___
#endif

#ifndef SSEUTIL_H___
#define SSEUTIL_H___

#ifndef __AVX__
typedef __m128i Mask;
typedef __m128i IVec;
typedef __m128 FVec;
#define AVXNAME(name) name##_AVX
#define SSENAME(name) name
#define SCALARNAME(name) name##_SCALAR
#else
typedef __m256i Mask;
typedef __m256i IVec;
typedef __m256 FVec;
#define AVXNAME(name) name
#define SSENAME(name) name##_SSE
#define SCALARNAME(name) name##_SCALAR
#define __SSE4__
#endif

#ifdef __GNUC__
#define __forceinline inline __attribute__((always_inline))
#endif

template <typename T>
static T convert(float a) {
  return std::floor(a + .5);
}

template <>
float convert<float>(float a) {
  return a;
}

template <>
double convert<double>(float a) {
  return a;
}

#ifdef __AVX__
__forceinline __m128i Load4Bytes(unsigned char* p) {
  return _mm_cvtepu8_epi32(_mm_cvtsi32_si128(*(int*)p));
}

__forceinline __m256i MakeInt8(__m128i a, __m128i b) {
  return _mm256_insertf128_si256(_mm256_castsi128_si256(a), b, 1);
}

__forceinline __m256 MakeFloat8(__m128 a, __m128 b) {
  return _mm256_insertf128_ps(_mm256_castps128_ps256(a), b, 1);
}

__forceinline __m128i LowInt4(__m256i a) {
  return _mm256_extractf128_si256(a, 0);
}

__forceinline __m128i HighInt4(__m256i a) {
  return _mm256_extractf128_si256(a, 1);
}

__forceinline __m128 LowFloat4(__m256 a) { return _mm256_extractf128_ps(a, 0); }

__forceinline __m128 HighFloat4(__m256 a) {
  return _mm256_extractf128_ps(a, 1);
}

__forceinline __m256i Load8Bytes(unsigned char* p) {
  return MakeInt8(Load4Bytes(p), Load4Bytes(p + 4));
}

__forceinline __m128 SSENAME(Load)(unsigned char* p) {
  return _mm_cvtepi32_ps(Load4Bytes(p));
}

__forceinline __m256 AVXNAME(Load)(unsigned char* p) {
  return _mm256_cvtepi32_ps(Load8Bytes(p));
}
#endif

__forceinline __m128 SSENAME(Load)(const float* p) { return _mm_loadu_ps(p); }

#ifdef __AVX__
__forceinline __m256 AVXNAME(Load)(const float* p) {
  return _mm256_loadu_ps(p);
}
#endif

__forceinline void SSENAME(Store)(float* p, __m128 src) {
  return _mm_storeu_ps(p, src);
}

#ifdef __AVX__
__forceinline void AVXNAME(Store)(float* p, __m256 src) {
  return _mm256_storeu_ps(p, src);
}

__forceinline __m128i SSENAME(LoadMask)(unsigned char* p) {
  __m128i a = Load4Bytes(p);
  __m128i zeros = _mm_set1_epi32(0);
  __m128i ones = _mm_set1_epi32(-1);
  return _mm_xor_si128(ones, _mm_cmpeq_epi32(a, zeros));
}

__forceinline __m256i AVXNAME(LoadMask)(unsigned char* p) {
  return MakeInt8(SSENAME(LoadMask)(p), SSENAME(LoadMask)(p + 4));
}
#endif

template <typename T>
__forceinline void SSENAME(Store)(T* p, __m128 src) {
  float psrc[4];
  _mm_storeu_ps(psrc, src);
  for (size_t i = 0; i < 4; i++) p[i] = convert<T>(psrc[i]);
}

#ifdef __AVX__
template <typename T>
__forceinline void AVXNAME(Store)(T* p, __m256 src) {
  SSENAME(Store)(p, LowFloat4(src));
  SSENAME(Store)(p + 4, HighFloat4(src));
}
#endif

__forceinline __m128i SSENAME(NonZero)(__m128i a) {
  __m128i zeros = _mm_set1_epi32(0);
  __m128i ones = _mm_set1_epi32(-1);
  return _mm_xor_si128(ones, _mm_cmpeq_epi32(a, zeros));
}

#ifdef __AVX__
__forceinline __m256i AVXNAME(NonZero)(__m256i a) {
  return MakeInt8(SSENAME(NonZero)(LowInt4(a)), SSENAME(NonZero)(HighInt4(a)));
}
#endif

#ifdef __SSE4__
__forceinline __m128 SSENAME(Blend)(__m128i mask, __m128 first, __m128 second) {
  return _mm_blendv_ps(second, first, _mm_castsi128_ps(mask));
}
#endif

#ifdef __AVX__
__forceinline __m256 AVXNAME(Blend)(__m256i mask, __m256 first, __m256 second) {
  return _mm256_blendv_ps(second, first, _mm256_castsi256_ps(mask));
}
#endif

#ifdef __SSE4__
__forceinline __m128 SSENAME(GatherDefault0)(float* p, __m128i idx,
                                             __m128i vmask) {
  __m128 r01 = _mm_set1_ps(0), r23 = _mm_set1_ps(0);
  int mask = _mm_movemask_epi8(vmask);

  if (mask & 1) r01 = _mm_set_ss(p[_mm_extract_epi32(idx, 0)]);

  if (mask & (1 << 8)) r23 = _mm_set_ss(p[_mm_extract_epi32(idx, 2)]);

  if (mask & (1 << 4))
    r01 = _mm_insert_ps(r01, _mm_set_ss(p[_mm_extract_epi32(idx, 1)]), 0x10);

  if (mask & (1 << 12))
    r23 = _mm_insert_ps(r23, _mm_set_ss(p[_mm_extract_epi32(idx, 3)]), 0x10);

  return _mm_unpacklo_ps(r01, r23);
}
#endif

#ifdef __AVX__
__forceinline __m256 AVXNAME(GatherDefault0)(float* p, __m256i idx,
                                             __m256i vmask) {
  return MakeFloat8(SSENAME(GatherDefault0)(p, LowInt4(idx), LowInt4(vmask)),
                    SSENAME(GatherDefault0)(p, HighInt4(idx), HighInt4(vmask)));
}
#endif

#ifdef __SSE4__
__forceinline __m128 SSENAME(Gather)(float* p, __m128i idx) {
  __m128 r01, r23;
  r01 = _mm_set_ss(p[_mm_extract_epi32(idx, 0)]);
  r23 = _mm_set_ss(p[_mm_extract_epi32(idx, 1)]);
  r01 = _mm_insert_ps(r01, _mm_set_ss(p[_mm_extract_epi32(idx, 2)]), 0x10);
  r23 = _mm_insert_ps(r23, _mm_set_ss(p[_mm_extract_epi32(idx, 3)]), 0x10);
  return _mm_unpacklo_ps(r01, r23);
}

#else
inline int get_x(const __m128i& vec) { return _mm_cvtsi128_si32(vec); }
inline int get_y(const __m128i& vec) {
  return _mm_cvtsi128_si32(_mm_shuffle_epi32(vec, 0x55));
}
inline int get_z(const __m128i& vec) {
  return _mm_cvtsi128_si32(_mm_shuffle_epi32(vec, 0xAA));
}
inline int get_w(const __m128i& vec) {
  return _mm_cvtsi128_si32(_mm_shuffle_epi32(vec, 0xFF));
}
__forceinline __m128 SSENAME(Gather)(float* p, __m128i idx) {
  return _mm_set_ps(p[get_w(idx)], p[get_z(idx)], p[get_y(idx)], p[get_x(idx)]);
}
#endif

template <typename T>
__forceinline __m128 SSENAME(Gather)(T* p, __m128i idx) {
  int pidx[4];
  float pr[4];

  _mm_storeu_si128((__m128i*)pidx, idx);

  for (size_t i = 0; i < 4; i++) pr[i] = float(p[pidx[i]]);

  return _mm_loadu_ps(pr);
}

#ifdef __AVX__
template <typename T>
__forceinline __m256 AVXNAME(Gather)(T* p, __m256i idx) {
  return MakeFloat8(SSENAME(Gather)(p, LowInt4(idx)),
                    SSENAME(Gather)(p, HighInt4(idx)));
}

__forceinline __m256 AVXNAME(Gather)(float* p, __m256i idx) {
  return MakeFloat8(SSENAME(Gather)(p, LowInt4(idx)),
                    SSENAME(Gather)(p, HighInt4(idx)));
}
#endif

#ifdef __SSE4__
__forceinline __m128i SSENAME(GatherIntDefault0)(unsigned char* p, __m128i idx,
                                                 __m128i vmask) {
  int mask = _mm_movemask_epi8(vmask);
  __m128i r01 = _mm_set1_epi32(0), r23 = _mm_set1_epi32(0);

  int i0 = _mm_extract_epi32(idx, 0);
  int i2 = _mm_extract_epi32(idx, 2);

  if (mask & (1 << 0)) r01 = _mm_cvtsi32_si128(p[i0]);

  if (mask & (1 << 8)) r23 = _mm_cvtsi32_si128(p[i2]);

  int i1 = _mm_extract_epi32(idx, 1);
  int i3 = _mm_extract_epi32(idx, 3);

  if (mask & (1 << 4)) r01 = _mm_insert_epi32(r01, p[i1], 0x1);

  if (mask & (1 << 12)) r23 = _mm_insert_epi32(r23, p[i3], 0x1);

  return _mm_unpacklo_epi64(r01, r23);
}
#endif

#ifdef __AVX__
__forceinline __m256i AVXNAME(GatherIntDefault0)(unsigned char* p, __m256i idx,
                                                 __m256i vmask) {
  return MakeInt8(
      SSENAME(GatherIntDefault0)(p, LowInt4(idx), LowInt4(vmask)),
      SSENAME(GatherIntDefault0)(p, HighInt4(idx), HighInt4(vmask)));
}
#endif

#ifdef __SSE4__
__forceinline void SSENAME(GatherIntDefault0_4)(
    unsigned char* p, __m128i vmask, __m128i* result0, __m128i* pidx0,
    __m128i* result1, __m128i* pidx1, __m128i* result2, __m128i* pidx2,
    __m128i* result3, __m128i* pidx3) {
  int mask = _mm_movemask_epi8(vmask);
  __m128i idx0 = *pidx0;
  __m128i idx1 = *pidx1;
  __m128i idx2 = *pidx2;
  __m128i idx3 = *pidx3;
  __m128i r0 = _mm_set1_epi32(0);
  __m128i r1 = _mm_set1_epi32(0);
  __m128i r2 = _mm_set1_epi32(0);
  __m128i r3 = _mm_set1_epi32(0);

  if (mask & (1 << 0)) {
    int i0 = _mm_extract_epi32(idx0, 0);
    int i1 = _mm_extract_epi32(idx1, 0);
    int i2 = _mm_extract_epi32(idx2, 0);
    int i3 = _mm_extract_epi32(idx3, 0);
    r0 = _mm_cvtsi32_si128(p[i0]);
    r1 = _mm_cvtsi32_si128(p[i1]);
    r2 = _mm_cvtsi32_si128(p[i2]);
    r3 = _mm_cvtsi32_si128(p[i3]);
  }

  if (mask & (1 << 4)) {
    int i0 = _mm_extract_epi32(idx0, 1);
    int i1 = _mm_extract_epi32(idx1, 1);
    int i2 = _mm_extract_epi32(idx2, 1);
    int i3 = _mm_extract_epi32(idx3, 1);
    r0 = _mm_insert_epi32(r0, p[i0], 1);
    r1 = _mm_insert_epi32(r1, p[i1], 1);
    r2 = _mm_insert_epi32(r2, p[i2], 1);
    r3 = _mm_insert_epi32(r3, p[i3], 1);
  }

  if (mask & (1 << 8)) {
    int i0 = _mm_extract_epi32(idx0, 2);
    int i1 = _mm_extract_epi32(idx1, 2);
    int i2 = _mm_extract_epi32(idx2, 2);
    int i3 = _mm_extract_epi32(idx3, 2);
    r0 = _mm_insert_epi32(r0, p[i0], 2);
    r1 = _mm_insert_epi32(r1, p[i1], 2);
    r2 = _mm_insert_epi32(r2, p[i2], 2);
    r3 = _mm_insert_epi32(r3, p[i3], 2);
  }

  if (mask & (1 << 12)) {
    int i0 = _mm_extract_epi32(idx0, 3);
    int i1 = _mm_extract_epi32(idx1, 3);
    int i2 = _mm_extract_epi32(idx2, 3);
    int i3 = _mm_extract_epi32(idx3, 3);
    r0 = _mm_insert_epi32(r0, p[i0], 3);
    r1 = _mm_insert_epi32(r1, p[i1], 3);
    r2 = _mm_insert_epi32(r2, p[i2], 3);
    r3 = _mm_insert_epi32(r3, p[i3], 3);
  }

  *result0 = r0;
  *result1 = r1;
  *result2 = r2;
  *result3 = r3;
}
#endif

#ifdef __AVX__
__forceinline void AVXNAME(GatherIntDefault0_4)(
    unsigned char* p, __m256i vmask, __m256i* result0, __m256i* pidx0,
    __m256i* result1, __m256i* pidx1, __m256i* result2, __m256i* pidx2,
    __m256i* result3, __m256i* pidx3) {
  __m128i lidx0 = LowInt4(*pidx0);
  __m128i lidx1 = LowInt4(*pidx1);
  __m128i lidx2 = LowInt4(*pidx2);
  __m128i lidx3 = LowInt4(*pidx3);
  __m128i lr0;
  __m128i lr1;
  __m128i lr2;
  __m128i lr3;
  SSENAME(GatherIntDefault0_4)(p, LowInt4(vmask), &lr0, &lidx0, &lr1, &lidx1,
                               &lr2, &lidx2, &lr3, &lidx3);

  __m128i hidx0 = HighInt4(*pidx0);
  __m128i hidx1 = HighInt4(*pidx1);
  __m128i hidx2 = HighInt4(*pidx2);
  __m128i hidx3 = HighInt4(*pidx3);
  __m128i hr0;
  __m128i hr1;
  __m128i hr2;
  __m128i hr3;
  SSENAME(GatherIntDefault0_4)(p, HighInt4(vmask), &hr0, &hidx0, &hr1, &hidx1,
                               &hr2, &hidx2, &hr3, &hidx3);

  *result0 = MakeInt8(lr0, hr0);
  *result1 = MakeInt8(lr1, hr1);
  *result2 = MakeInt8(lr2, hr2);
  *result3 = MakeInt8(lr3, hr3);
}
#endif

#ifdef __SSE4__
__forceinline __m128i SSENAME(GatherInt)(unsigned char* p, __m128i idx) {
  __m128i r01, r23;

  int i0 = _mm_extract_epi32(idx, 0);
  int i2 = _mm_extract_epi32(idx, 2);
  r01 = _mm_cvtsi32_si128(p[i0]);
  r23 = _mm_cvtsi32_si128(p[i2]);
  int i1 = _mm_extract_epi32(idx, 1);
  int i3 = _mm_extract_epi32(idx, 3);
  r01 = _mm_insert_epi32(r01, p[i1], 0x1);
  r23 = _mm_insert_epi32(r23, p[i3], 0x1);

  return _mm_unpacklo_epi64(r01, r23);
}
#endif

#ifdef __AVX__
__forceinline __m256i AVXNAME(GatherInt)(unsigned char* p, __m256i idx) {
  return MakeInt8(SSENAME(GatherInt)(p, LowInt4(idx)),
                  SSENAME(GatherInt)(p, HighInt4(idx)));
}
#endif

#ifdef __SSE4__
__forceinline __m128 SSENAME(Gather)(unsigned char* p, __m128i idx) {
  return _mm_cvtepi32_ps(SSENAME(GatherInt)(p, idx));
}
#endif

#ifdef __AVX__
__forceinline __m256 AVXNAME(Gather)(unsigned char* p, __m256i idx) {
  return _mm256_cvtepi32_ps(AVXNAME(GatherInt)(p, idx));
}
#endif

#ifdef __SSE4__
__forceinline __m128 SSENAME(GatherDefault0)(unsigned char* p, __m128i idx,
                                             __m128i vmask) {
  return _mm_cvtepi32_ps(SSENAME(GatherIntDefault0)(p, idx, vmask));
}
#endif

#ifdef __AVX__
__forceinline __m256 AVXNAME(GatherDefault0)(unsigned char* p, __m256i idx,
                                             __m256i vmask) {
  return _mm256_cvtepi32_ps(AVXNAME(GatherIntDefault0)(p, idx, vmask));
}

#define SSE_INT_BINARY_TO_AVX(name)                           \
  __forceinline __m256i AVXNAME(name)(__m256i a, __m256i b) { \
    return MakeInt8(SSENAME(name)(LowInt4(a), LowInt4(b)),    \
                    SSENAME(name)(HighInt4(a), HighInt4(b))); \
  }
#endif

__forceinline __m128 SSENAME(Min)(__m128 a, __m128 b) {
  return _mm_min_ps(a, b);
}

#ifdef __AVX__
__forceinline __m256 AVXNAME(Min)(__m256 a, __m256 b) {
  return _mm256_min_ps(a, b);
}
#endif

__forceinline __m128 SSENAME(Max)(__m128 a, __m128 b) {
  return _mm_max_ps(a, b);
}

#ifdef __AVX__
__forceinline __m256 AVXNAME(Max)(__m256 a, __m256 b) {
  return _mm256_max_ps(a, b);
}
#endif

__forceinline __m128i SSENAME(CmpLt)(__m128i a, __m128i b) {
  return _mm_cmplt_epi32(a, b);
}

__forceinline __m128i SSENAME(CmpGe)(__m128i a, __m128i b) {
  return _mm_xor_si128(_mm_set1_epi32(-1), _mm_cmplt_epi32(a, b));
}

__forceinline __m128i SSENAME(CmpEq)(__m128i a, __m128i b) {
  return _mm_cmpeq_epi32(a, b);
}

__forceinline __m128i SSENAME(CmpNe)(__m128i a, __m128i b) {
  return _mm_xor_si128(_mm_set1_epi32(-1), _mm_cmpeq_epi32(a, b));
}

#ifdef __AVX__
SSE_INT_BINARY_TO_AVX(CmpLt)
SSE_INT_BINARY_TO_AVX(CmpGe)
SSE_INT_BINARY_TO_AVX(CmpEq)
SSE_INT_BINARY_TO_AVX(CmpNe)
#endif

__forceinline __m128i SSENAME(CmpGt)(__m128 a, __m128 b) {
  return _mm_castps_si128(_mm_cmpgt_ps(a, b));
}

#ifdef __AVX__
__forceinline __m256i AVXNAME(CmpGt)(__m256 a, __m256 b) {
  return _mm256_castps_si256(_mm256_cmp_ps(a, b, 0xe));
}
#endif

#ifdef __SSE4__
__forceinline int SSENAME(AllEqual)(__m128i a, __m128i b) {
  __m128i eq = _mm_xor_si128(a, b);
  return _mm_testz_si128(eq, eq);
}
#endif

#ifdef __AVX__
__forceinline int AVXNAME(AllEqual)(__m256i a, __m256i b) {
  return SSENAME(AllEqual)(LowInt4(a), LowInt4(b)) &&
         SSENAME(AllEqual)(HighInt4(a), HighInt4(b));
}
#endif

#ifdef __SSE4__
__forceinline int SSENAME(AllZero)(__m128i a) { return _mm_testz_si128(a, a); }
#endif

#ifdef __AVX__
__forceinline int AVXNAME(AllZero)(__m256i a) {
  return _mm256_testz_si256(a, a);
}
#endif

#ifdef __SSE4__
__forceinline int SSENAME(AllSet)(__m128i a) {
  __m128i ones;
  ones = _mm_cmpeq_epi32(ones, ones);
  return _mm_testc_si128(a, ones);
}
#endif

#ifdef __AVX__
__forceinline int AVXNAME(AllSet)(__m256i a) {
  return SSENAME(AllSet)(LowInt4(a)) && SSENAME(AllSet)(HighInt4(a));
}
#endif

__forceinline __m128i SSENAME(Add)(__m128i a, __m128i b) {
  return _mm_add_epi32(a, b);
}
__forceinline __m128i SSENAME(Sub)(__m128i a, __m128i b) {
  return _mm_sub_epi32(a, b);
}

#ifdef __SSE4__
__forceinline __m128i SSENAME(Mul)(__m128i a, __m128i b) {
  return _mm_mullo_epi32(a, b);
}
#endif

#ifdef __AVX__
SSE_INT_BINARY_TO_AVX(Add)
SSE_INT_BINARY_TO_AVX(Sub)
SSE_INT_BINARY_TO_AVX(Mul)
#endif

__forceinline __m128 SSENAME(Add)(__m128 a, __m128 b) {
  return _mm_add_ps(a, b);
}
__forceinline __m128 SSENAME(Sub)(__m128 a, __m128 b) {
  return _mm_sub_ps(a, b);
}
__forceinline __m128 SSENAME(Mul)(__m128 a, __m128 b) {
  return _mm_mul_ps(a, b);
}
__forceinline __m128 SSENAME(Div)(__m128 a, __m128 b) {
  return _mm_div_ps(a, b);
}

#ifdef __AVX__
__forceinline __m256 AVXNAME(Add)(__m256 a, __m256 b) {
  return _mm256_add_ps(a, b);
}
__forceinline __m256 AVXNAME(Sub)(__m256 a, __m256 b) {
  return _mm256_sub_ps(a, b);
}
__forceinline __m256 AVXNAME(Mul)(__m256 a, __m256 b) {
  return _mm256_mul_ps(a, b);
}
__forceinline __m256 AVXNAME(Div)(__m256 a, __m256 b) {
  return _mm256_div_ps(a, b);
}
#endif

__forceinline __m128i SSENAME(And)(__m128i a, __m128i b) {
  return _mm_and_si128(a, b);
}
__forceinline __m128i SSENAME(AndNot)(__m128i a, __m128i b) {
  return _mm_andnot_si128(b, a);
}
__forceinline __m128i SSENAME(Or)(__m128i a, __m128i b) {
  return _mm_or_si128(a, b);
}

#ifdef __AVX__
SSE_INT_BINARY_TO_AVX(And)
SSE_INT_BINARY_TO_AVX(AndNot)
SSE_INT_BINARY_TO_AVX(Or)
#endif

__forceinline __m128i SSENAME(ToInt)(__m128 a) { return _mm_cvtps_epi32(a); }

#ifdef __AVX__
__forceinline __m256i AVXNAME(ToInt)(__m256 a) {
  return MakeInt8(SSENAME(ToInt)(LowFloat4(a)), SSENAME(ToInt)(HighFloat4(a)));
}
#endif

__forceinline __m128 SSENAME(ToFloat)(__m128i a) { return _mm_cvtepi32_ps(a); }

#ifdef __AVX__
__forceinline __m256 AVXNAME(ToFloat)(__m256i a) {
  return MakeFloat8(SSENAME(ToFloat)(LowInt4(a)),
                    SSENAME(ToFloat)(HighInt4(a)));
}
#endif

#ifdef __SSE4__
__forceinline __m128 SSENAME(Floor)(__m128 a) { return _mm_floor_ps(a); }
#else
inline __m128 SSENAME(Floor)(__m128 a) {
  static const __m128 one = _mm_set1_ps(1.0f);
  __m128 fval = _mm_cvtepi32_ps(_mm_cvttps_epi32(a));
  return _mm_sub_ps(fval, _mm_and_ps(_mm_cmplt_ps(a, fval), one));
}
#endif

#ifdef __AVX__
__forceinline __m256 AVXNAME(Floor)(__m256 a) {
  return MakeFloat8(SSENAME(Floor)(LowFloat4(a)),
                    SSENAME(Floor)(HighFloat4(a)));
}
#endif

__forceinline __m128i SSENAME(Set1)(int a) { return _mm_set1_epi32(a); }

#ifdef __AVX__
__forceinline __m256i AVXNAME(Set1)(int a) { return _mm256_set1_epi32(a); }
#endif

__forceinline __m128 SSENAME(Set1)(float a) { return _mm_set1_ps(a); }

#ifdef __AVX__
__forceinline __m256 AVXNAME(Set1)(float a) { return _mm256_set1_ps(a); }
#endif

template <typename T>
__forceinline T SCALARNAME(Blend)(int cond, T first, T second) {
  return cond ? first : second;
}

inline int SCALARNAME(getMask)(bool cond) { return cond ? 1 : 0; }

template <typename T>
__forceinline float SCALARNAME(Gather)(float src, T* p, size_t idx, int mask) {
  return mask ? (float)p[idx] : src;
}

template <typename T>
__forceinline int SCALARNAME(Gather)(int src, T* p, size_t idx, int mask) {
  return mask ? (int)p[idx] : src;
}

template <typename T>
__forceinline float SCALARNAME(GatherDefault0)(T* p, size_t idx, int mask) {
  return mask ? (float)p[idx] : 0.0f;
}

template <typename T>
__forceinline int SCALARNAME(GatherIntDefault0)(T* p, size_t idx, int mask) {
  return mask ? (int)p[idx] : 0;
}

template <typename T>
__forceinline float SCALARNAME(Gather)(T* p, size_t idx) {
  return (float)p[idx];
}

template <typename T>
__forceinline int SCALARNAME(GatherInt)(T* p, size_t idx) {
  return (int)p[idx];
}

__forceinline int SCALARNAME(CmpGe)(int a, int b) { return a >= b ? 1 : 0; }
__forceinline int SCALARNAME(CmpLt)(int a, int b) { return a < b ? 1 : 0; }
__forceinline int SCALARNAME(CmpEq)(int a, int b) { return a == b ? 1 : 0; }
__forceinline int SCALARNAME(CmpNe)(int a, int b) { return a != b ? 1 : 0; }

__forceinline int SCALARNAME(CmpGt)(float a, float b) { return a > b ? 1 : 0; }

__forceinline bool SCALARNAME(AllEqual)(int a, int b) { return a == b; }
__forceinline bool SCALARNAME(AllZero)(int a) { return a == 0; }

__forceinline int SCALARNAME(And)(int a, int b) { return a & b; }
__forceinline int SCALARNAME(AndNot)(int a, int b) { return a & ~b; }
__forceinline int SCALARNAME(Or)(int a, int b) { return a | b; }

__forceinline int SCALARNAME(ToInt)(float a) { return (int)a; }
__forceinline float SCALARNAME(ToFloat)(int a) { return a; }

__forceinline int SCALARNAME(Add)(int a, int b) { return a + b; }
__forceinline int SCALARNAME(Sub)(int a, int b) { return a - b; }
__forceinline int SCALARNAME(Mul)(int a, int b) { return a * b; }

__forceinline float SCALARNAME(Add)(float a, float b) { return a + b; }
__forceinline float SCALARNAME(Sub)(float a, float b) { return a - b; }
__forceinline float SCALARNAME(Mul)(float a, float b) { return a * b; }
__forceinline float Div(float a, float b) { return a / b; }

template <typename T>
__forceinline T SCALARNAME(Add3)(T a, T b, T c) {
  return SCALARNAME(Add)(a, SCALARNAME(Add)(b, c));
}

template <typename T>
__forceinline T SCALARNAME(Mul3)(T a, T b, T c) {
  return SCALARNAME(Mul)(a, SCALARNAME(Mul)(b, c));
}

#define BINARY_FUN_TO_3ARG(fbin, f3arg, type)        \
  __forceinline type f3arg(type a, type b, type c) { \
    return fbin(a, fbin(b, c));                      \
  }

BINARY_FUN_TO_3ARG(SSENAME(Add), SSENAME(Add3), __m128)
BINARY_FUN_TO_3ARG(SSENAME(Add), SSENAME(Add3), __m128i)
BINARY_FUN_TO_3ARG(SSENAME(Mul), SSENAME(Mul3), __m128)

#ifdef __SSE4__
BINARY_FUN_TO_3ARG(SSENAME(Mul), SSENAME(Mul3), __m128i)
#endif

#ifdef __AVX__
BINARY_FUN_TO_3ARG(AVXNAME(Add), AVXNAME(Add3), __m256)
BINARY_FUN_TO_3ARG(AVXNAME(Add), AVXNAME(Add3), __m256i)
BINARY_FUN_TO_3ARG(AVXNAME(Mul), AVXNAME(Mul3), __m256)
BINARY_FUN_TO_3ARG(AVXNAME(Mul), AVXNAME(Mul3), __m256i)
#endif

__forceinline int SCALARNAME(Set1)(int a) { return a; }
__forceinline float SCALARNAME(Set1)(float a) { return a; }
__forceinline int SCALARNAME(LoadMask)(unsigned char* p) { return *p; }
__forceinline float SCALARNAME(Load)(unsigned char* p) { return *p; }
__forceinline float SCALARNAME(Load)(float* p) { return *p; }

#endif
