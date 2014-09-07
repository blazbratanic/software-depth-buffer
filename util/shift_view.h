#ifndef SHIFT_VIEW_H
#define SHIFT_VIEW_H

#include <util/array2dview.h>

// namespace sil {
template <typename T>
Array2dView<T> shift_view(Array2dView<T> const& view, int rT, int cT) {
  Array2dView<T> shifted_view;

  if (rT < 0)
    shifted_view = get_last_n_rows(view, view.rows() + rT);
  else
    shifted_view = get_first_n_rows(view, view.rows() - rT);

  if (cT < 0)
    shifted_view = get_last_n_cols(shifted_view, view.cols() + cT);
  else
    shifted_view = get_first_n_cols(shifted_view, view.cols() - cT);

  return shifted_view;
}

template <typename T>
ConstArray2dView<T> shift_view(ConstArray2dView<T> const& view, int rT,
                               int cT) {
  Array2dView<T> mutable_array(view.GetData(), view.GetSize0(), view.GetSize1(),
                               view.GetSize1());
  return ConstArray2dView<T>(shift_view(mutable_array, rT, cT));
}
//}

#endif
