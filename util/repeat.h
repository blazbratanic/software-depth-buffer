#ifndef REPEAT_H
#define REPEAT_H

#include <util/array2d.h>
#include <util/array2dview.h>

namespace sil {

template <typename T>
Array2dView<T> repeat(ConstArray2dView<T> src, Array2dView<T> dst, size_t nrow,
                      size_t ncol) {

  size_t minrow = std::min(src.rows(), dst.rows() / nrow);
  size_t mincol = std::min(src.cols(), dst.cols() / ncol);

  for (size_t r = 0; r < minrow; ++r) {
    for (size_t rr = 0; rr < nrow; ++rr) {
      size_t current_row = nrow * r + rr;
      for (size_t c = 0; c < mincol; ++c) {
        for (size_t rc = 0; rc < ncol; ++rc) {
          dst(current_row, ncol* c + rc) = src(r, c);
        }
      }
    }
  }

  return dst;
}

template <typename T>
Array2d<T> repeat(Array2d<T> src, Array2d<T> dst, size_t nrow, size_t ncol) {
  dst.Allocate(src.rows() * nrow, src.cols() * ncol);
  return repeat(ConstArray2dView<T>(src), Array2dView<T>(dst), nrow, ncol);
}

template <typename T>
Array2dView<T> repeat(ConstArray2dView<T> src, Array2dView<T> dst, size_t n) {
  return repeat(src, dst, n, n);
}
}  // sil

#endif
