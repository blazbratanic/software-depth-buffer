
struct Roi {
  Roi(size_t lu_row, size_t lu_col, size_t rb_row, size_t rb_col) {
    left_upper = PixelPos(lu_row, lu_col);
    right_bottom = PixelPos(rb_row, rb_col);
  }

  Roi(PixelPos const &lu, PixelPos const &rb) {
    left_upper = lu;
    right_bottom = rb;
  }

  PixelPos left_upper;
  PixelPos right_bottom;
};

inline Roi intersection(Roi a, Roi b) {
  return Roi(std::max(a.left_upper.row, b.left_upper.row),
             std::max(a.left_upper.col, b.left_upper.col),
             std::min(a.right_bottom.row, b.right_bottom.row),
             std::min(a.right_bottom.col, b.right_bottom.col));
}
