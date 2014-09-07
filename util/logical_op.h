#ifndef LOGICAL_OP_H
#define LOGICAL_OP_H

#include <util/array2dview.h>
#include <util/array2dview_op.h>

namespace sil {

template <typename T>
Array2dView<T> bitwise_logical_or(ConstArray2dView<T> src1,
                                  ConstArray2dView<T> src2,
                                  Array2dView<T> dst) {
  //bitwise_or_sse(src1, src2, dst);
  sil::transform(src1, src2, dst, [](T a, T b) { return a | b; });
  return dst;
}
}

#endif
