#ifndef FIXED_SIZE_MATRIX_H_
#define FIXED_SIZE_MATRIX_H_

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cmath>
#include <cassert>

namespace sil {

template <typename Ty, int R, int C = R>
struct FixedSizeMatrix {
  typedef Ty value_type;
  static const int Rows = R;
  static const int Cols = C;

  union {
    struct {
      Ty element[R][C];
    };
    struct {
      Ty flatten[R * C];
    };
  };

  Ty &operator[](int i) { return flatten[i]; }
  const Ty &operator[](int i) const { return flatten[i]; }

  // Access without bounds checking
  Ty &operator()(int r, int c) { return element[r][c]; }
  const Ty &operator()(int r, int c) const { return element[r][c]; }

  // Access with bounds checking
  Ty &at(int r, int c) {
    if (!(r >= 0 && r < R && c >= 0 && c < C))
      throw std::out_of_range("Out of range index.");
    return element[r][c];
  }

  const Ty &at(int r, int c) const {
    if (!(r >= 0 && r < R && c >= 0 && c < C))
      throw std::out_of_range("Out of range index.");
    return element[r][c];
  }

  FixedSizeMatrix<Ty, C, R> transpose() const {
    FixedSizeMatrix<Ty, C, R> result;
    for (int r = 0; r < R; ++r) {
      for (int c = 0; c < C; ++c) {
        result.element[c][r] = element[r][c];
      }
    }
    return result;
  }

  FixedSizeMatrix<Ty, R, C> inverse() const;
};

///
// Matrix Inverse
///
// A design choice was made to keep FixedSizeMatrix an aggregate class to enable
// FixedSizeMatrix<float, 2, 2> = {1.0f, 1.0f, 0.5f, 0.2f} initialization.
// With c++11 it would be possible to create a FixedSizeMatrix base class
// and derive all variations from it, while retaining the brace
// initialization.
//

#include "detail/fixed_size_matrix_inverse.impl"

// Define matrix inverse
template <typename Ty, int R, int C>
FixedSizeMatrix<Ty, R, C> FixedSizeMatrix<Ty, R, C>::inverse() const {
  return matrix::inverse<Ty, R, C>()(*this);
}

///
//  Matrix operations
///
// Matrix product
template <typename Ty, int R, int C, int P>
FixedSizeMatrix<Ty, R, P> operator*(FixedSizeMatrix<Ty, R, C> const &a,
                                    FixedSizeMatrix<Ty, C, P> const &b) {
  FixedSizeMatrix<Ty, R, P> result;

  for (int r = 0; r < R; ++r) {
    for (int c = 0; c < P; ++c) {
      Ty accum = Ty(0);
      for (int i = 0; i < C; ++i) {
        accum += a.element[r][i] * b.element[i][c];
      }
      result.element[r][c] = accum;
    }
  }
  return result;
}

// Vector dot product
template <typename Ty, int R>
typename std::enable_if<(R > 1), Ty>::type operator*(
    FixedSizeMatrix<Ty, R, 1> const &a, FixedSizeMatrix<Ty, R, 1> const &b) {
  Ty result = Ty();

  for (int i = 0; i < R; i++) result += a.flatten[i] * b.flatten[i];

  return result;
}

// Unary negation
template <typename Ty, int R, int C>
FixedSizeMatrix<Ty, R, C> operator-(FixedSizeMatrix<Ty, R, C> const &a) {
  FixedSizeMatrix<Ty, R, C> result;
  for (int e = 0; e < R * C; ++e) result.flatten[e] = -a.flatten[e];
  return result;
}

#define MATRIX_WITH_MATRIX_OPERATOR(op_symbol, op)      \
  template <typename Ty, int R, int C>                  \
  FixedSizeMatrix<Ty, R, C> operator op_symbol(         \
      FixedSizeMatrix<Ty, R, C> const &a,               \
      FixedSizeMatrix<Ty, R, C> const &b) {             \
    FixedSizeMatrix<Ty, R, C> result;                   \
    for (int e = 0; e < R * C; ++e)                     \
      result.flatten[e] = a.flatten[e] op b.flatten[e]; \
    return result;                                      \
  }

MATRIX_WITH_MATRIX_OPERATOR(+, +);  // Matrix addition
MATRIX_WITH_MATRIX_OPERATOR(-, -);  // Matrix subtraction
#undef MATRIX_WITH_MATRIX_OPERATOR

#define MATRIX_WITH_SCALAR_OPERATOR(op_symbol, op)     \
  template <typename Ty, int R, int C>                 \
  FixedSizeMatrix<Ty, R, C> operator op_symbol(        \
      FixedSizeMatrix<Ty, R, C> const &a, Ty scalar) { \
    FixedSizeMatrix<Ty, R, C> result;                  \
    for (int e = 0; e < R * C; ++e)                    \
        result.flatten[e] = a.flatten[e] op scalar;    \
    return result;                                     \
  }

// Add scalar to matrix
MATRIX_WITH_SCALAR_OPERATOR(+, +);
// Subtract scalar from matrix
MATRIX_WITH_SCALAR_OPERATOR(-, -);
// Multiply scalar with matrix
MATRIX_WITH_SCALAR_OPERATOR(*, *);
// Divide scalar with matrix
MATRIX_WITH_SCALAR_OPERATOR(/, / );
#undef MATRIX_WITH_SCALAR_OPERATOR

// Add matrix and scalar - commutative
template <typename Ty, int R, int C>
FixedSizeMatrix<Ty, R, C> operator+(Ty scalar,
                                    FixedSizeMatrix<Ty, R, C> const &a) {
  return a + scalar;
}

// Multiply matrix and scalar - commutative
template <typename Ty, int R, int C>
FixedSizeMatrix<Ty, R, C> operator*(Ty scalar,
                                    FixedSizeMatrix<Ty, R, C> const &a) {
  return a * scalar;
}

// Multiply matrix and scalar - noncommutative
template <typename Ty, int R, int C>
FixedSizeMatrix<Ty, R, C> operator-(Ty scalar,
                                    FixedSizeMatrix<Ty, R, C> const &a) {
  return -a + scalar;
}

template <typename Ty, int R, int C>
bool operator==(FixedSizeMatrix<Ty, R, C> const &a,
                FixedSizeMatrix<Ty, R, C> const &b) {

  for (int i = 0; i < R * C; i++) {
    if (std::abs(a.flatten[i] - b.flatten[i]) > 1e-6) return false;
  }

  return true;
}

// TODO Divide matrix and scalar - noncommutative

///
// Helper functions
///
namespace matrix {
// Create identity matrix
template <typename Ty, int R>
FixedSizeMatrix<Ty, R, R> identity() {
  FixedSizeMatrix<Ty, R, R> result =
      FixedSizeMatrix<Ty, R, R>();  // zero initialization,
                                    // due to value
                                    // initialization
  for (int i = 0; i < R; ++i) result.element[i][i] = Ty(1);
  return result;
}
}

///
// IO operations
///
template <typename Ty, int R, int C>
std::ostream &operator<<(std::ostream &out,
                         FixedSizeMatrix<Ty, R, C> const &a) {
  for (int r = 0; r < R; ++r) {
    for (int c = 0; c < C; ++c) {
      out << a.element[r][c] << " ";
    }
    out << std::endl;
  }
  return out;
}

typedef FixedSizeMatrix<float, 2, 2> Mat2f;
typedef FixedSizeMatrix<float, 3, 3> Mat3f;
typedef FixedSizeMatrix<float, 4, 4> Mat4f;

template <typename Ty, int R>
using FixedSizeVector = FixedSizeMatrix<float, R, 1>;

template <typename Ty, int R>
float norm(FixedSizeMatrix<Ty, R, 1> const &a) {
  return std::sqrt(a * a);
}

template <typename Ty, int R>
float distance(FixedSizeMatrix<Ty, R, 1> const &a,
               FixedSizeMatrix<Ty, R, 1> const &b) {
  return norm(a - b);
}

template <typename Ty, int R>
FixedSizeVector<Ty, R> normalize(FixedSizeMatrix<Ty, R, 1> const &a) {
  assert(norm<Ty>(a) != 0.0f);

  return a / norm<Ty>(a);
}

template <typename Ty, int R>
float angle(FixedSizeMatrix<Ty, R, 1> const &a, FixedSizeMatrix<Ty, R, 1> const &b) {
  assert(norm<Ty>(a) != Ty(0));
  assert(norm<Ty>(b) != Ty(0));
  return std::acos(a * b - 1e-6f) / (norm<Ty>(a) * norm<Ty>(b));
}
typedef FixedSizeVector<int, 2> Vec2i;
typedef FixedSizeVector<int, 3> Vec3i;
typedef FixedSizeVector<int, 4> Vec4i;

typedef FixedSizeVector<float, 2> Vec2f;
typedef FixedSizeVector<float, 3> Vec3f;
typedef FixedSizeVector<float, 4> Vec4f;

}  // sil

#endif
