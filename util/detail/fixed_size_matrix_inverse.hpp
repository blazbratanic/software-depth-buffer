// Matrix inverse helpers
namespace matrix {
// General version not implemented
template <typename Ty, int R, int C>
struct inverse;

// Matrix inversion for 2x2 matrix
template <typename Ty>
struct inverse<Ty, 2, 2> {
  FixedSizeMatrix<Ty, 2, 2> operator()(FixedSizeMatrix<Ty, 2, 2> const &a) {
    FixedSizeMatrix<Ty, 2, 2> result;
    const Ty det =
        a.element[0][0] * a.element[1][1] - a.element[0][1] * a.element[1][0];
    if (det == 0) {
      throw std::runtime_error("Singular matrix");
    }

    result.element[0][0] = a.element[1][1] / det;
    result.element[1][1] = a.element[0][0] / det;
    result.element[0][1] = -a.element[0][1] / det;
    result.element[1][0] = -a.element[1][0] / det;
    return result;
  }
};

// Matrix inversion for 4x4 matrix
template <typename Ty>
struct inverse<Ty, 4, 4> {
  FixedSizeMatrix<Ty, 4, 4> operator()(FixedSizeMatrix<Ty, 4, 4> const &m) {
    FixedSizeMatrix<Ty, 4, 4> inv;

    inv.flatten[0] = m.flatten[5] * m.flatten[10] * m.flatten[15] -
                     m.flatten[5] * m.flatten[11] * m.flatten[14] -
                     m.flatten[9] * m.flatten[6] * m.flatten[15] +
                     m.flatten[9] * m.flatten[7] * m.flatten[14] +
                     m.flatten[13] * m.flatten[6] * m.flatten[11] -
                     m.flatten[13] * m.flatten[7] * m.flatten[10];

    inv.flatten[4] = -m.flatten[4] * m.flatten[10] * m.flatten[15] +
                     m.flatten[4] * m.flatten[11] * m.flatten[14] +
                     m.flatten[8] * m.flatten[6] * m.flatten[15] -
                     m.flatten[8] * m.flatten[7] * m.flatten[14] -
                     m.flatten[12] * m.flatten[6] * m.flatten[11] +
                     m.flatten[12] * m.flatten[7] * m.flatten[10];

    inv.flatten[8] = m.flatten[4] * m.flatten[9] * m.flatten[15] -
                     m.flatten[4] * m.flatten[11] * m.flatten[13] -
                     m.flatten[8] * m.flatten[5] * m.flatten[15] +
                     m.flatten[8] * m.flatten[7] * m.flatten[13] +
                     m.flatten[12] * m.flatten[5] * m.flatten[11] -
                     m.flatten[12] * m.flatten[7] * m.flatten[9];

    inv.flatten[12] = -m.flatten[4] * m.flatten[9] * m.flatten[14] +
                      m.flatten[4] * m.flatten[10] * m.flatten[13] +
                      m.flatten[8] * m.flatten[5] * m.flatten[14] -
                      m.flatten[8] * m.flatten[6] * m.flatten[13] -
                      m.flatten[12] * m.flatten[5] * m.flatten[10] +
                      m.flatten[12] * m.flatten[6] * m.flatten[9];

    inv.flatten[1] = -m.flatten[1] * m.flatten[10] * m.flatten[15] +
                     m.flatten[1] * m.flatten[11] * m.flatten[14] +
                     m.flatten[9] * m.flatten[2] * m.flatten[15] -
                     m.flatten[9] * m.flatten[3] * m.flatten[14] -
                     m.flatten[13] * m.flatten[2] * m.flatten[11] +
                     m.flatten[13] * m.flatten[3] * m.flatten[10];

    inv.flatten[5] = m.flatten[0] * m.flatten[10] * m.flatten[15] -
                     m.flatten[0] * m.flatten[11] * m.flatten[14] -
                     m.flatten[8] * m.flatten[2] * m.flatten[15] +
                     m.flatten[8] * m.flatten[3] * m.flatten[14] +
                     m.flatten[12] * m.flatten[2] * m.flatten[11] -
                     m.flatten[12] * m.flatten[3] * m.flatten[10];

    inv.flatten[9] = -m.flatten[0] * m.flatten[9] * m.flatten[15] +
                     m.flatten[0] * m.flatten[11] * m.flatten[13] +
                     m.flatten[8] * m.flatten[1] * m.flatten[15] -
                     m.flatten[8] * m.flatten[3] * m.flatten[13] -
                     m.flatten[12] * m.flatten[1] * m.flatten[11] +
                     m.flatten[12] * m.flatten[3] * m.flatten[9];

    inv.flatten[13] = m.flatten[0] * m.flatten[9] * m.flatten[14] -
                      m.flatten[0] * m.flatten[10] * m.flatten[13] -
                      m.flatten[8] * m.flatten[1] * m.flatten[14] +
                      m.flatten[8] * m.flatten[2] * m.flatten[13] +
                      m.flatten[12] * m.flatten[1] * m.flatten[10] -
                      m.flatten[12] * m.flatten[2] * m.flatten[9];

    inv.flatten[2] = m.flatten[1] * m.flatten[6] * m.flatten[15] -
                     m.flatten[1] * m.flatten[7] * m.flatten[14] -
                     m.flatten[5] * m.flatten[2] * m.flatten[15] +
                     m.flatten[5] * m.flatten[3] * m.flatten[14] +
                     m.flatten[13] * m.flatten[2] * m.flatten[7] -
                     m.flatten[13] * m.flatten[3] * m.flatten[6];

    inv.flatten[6] = -m.flatten[0] * m.flatten[6] * m.flatten[15] +
                     m.flatten[0] * m.flatten[7] * m.flatten[14] +
                     m.flatten[4] * m.flatten[2] * m.flatten[15] -
                     m.flatten[4] * m.flatten[3] * m.flatten[14] -
                     m.flatten[12] * m.flatten[2] * m.flatten[7] +
                     m.flatten[12] * m.flatten[3] * m.flatten[6];

    inv.flatten[10] = m.flatten[0] * m.flatten[5] * m.flatten[15] -
                      m.flatten[0] * m.flatten[7] * m.flatten[13] -
                      m.flatten[4] * m.flatten[1] * m.flatten[15] +
                      m.flatten[4] * m.flatten[3] * m.flatten[13] +
                      m.flatten[12] * m.flatten[1] * m.flatten[7] -
                      m.flatten[12] * m.flatten[3] * m.flatten[5];

    inv.flatten[14] = -m.flatten[0] * m.flatten[5] * m.flatten[14] +
                      m.flatten[0] * m.flatten[6] * m.flatten[13] +
                      m.flatten[4] * m.flatten[1] * m.flatten[14] -
                      m.flatten[4] * m.flatten[2] * m.flatten[13] -
                      m.flatten[12] * m.flatten[1] * m.flatten[6] +
                      m.flatten[12] * m.flatten[2] * m.flatten[5];

    inv.flatten[3] = -m.flatten[1] * m.flatten[6] * m.flatten[11] +
                     m.flatten[1] * m.flatten[7] * m.flatten[10] +
                     m.flatten[5] * m.flatten[2] * m.flatten[11] -
                     m.flatten[5] * m.flatten[3] * m.flatten[10] -
                     m.flatten[9] * m.flatten[2] * m.flatten[7] +
                     m.flatten[9] * m.flatten[3] * m.flatten[6];

    inv.flatten[7] = m.flatten[0] * m.flatten[6] * m.flatten[11] -
                     m.flatten[0] * m.flatten[7] * m.flatten[10] -
                     m.flatten[4] * m.flatten[2] * m.flatten[11] +
                     m.flatten[4] * m.flatten[3] * m.flatten[10] +
                     m.flatten[8] * m.flatten[2] * m.flatten[7] -
                     m.flatten[8] * m.flatten[3] * m.flatten[6];

    inv.flatten[11] = -m.flatten[0] * m.flatten[5] * m.flatten[11] +
                      m.flatten[0] * m.flatten[7] * m.flatten[9] +
                      m.flatten[4] * m.flatten[1] * m.flatten[11] -
                      m.flatten[4] * m.flatten[3] * m.flatten[9] -
                      m.flatten[8] * m.flatten[1] * m.flatten[7] +
                      m.flatten[8] * m.flatten[3] * m.flatten[5];

    inv.flatten[15] = m.flatten[0] * m.flatten[5] * m.flatten[10] -
                      m.flatten[0] * m.flatten[6] * m.flatten[9] -
                      m.flatten[4] * m.flatten[1] * m.flatten[10] +
                      m.flatten[4] * m.flatten[2] * m.flatten[9] +
                      m.flatten[8] * m.flatten[1] * m.flatten[6] -
                      m.flatten[8] * m.flatten[2] * m.flatten[5];

    const Ty det =
        m.flatten[0] * inv.flatten[0] + m.flatten[1] * inv.flatten[4] +
        m.flatten[2] * inv.flatten[8] + m.flatten[3] * inv.flatten[12];

    if (det == 0) {
      throw std::runtime_error("Singular matrix");
    }

    det = Ty(1) / det;
    inv = inv * det;

    return inv;
  }
};
}  // matrix
