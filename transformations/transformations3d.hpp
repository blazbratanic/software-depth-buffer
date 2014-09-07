#ifndef TRANSFORMATIONS_3D_HPP_
#define TRANSFORMATIONS_3D_HPP_

#include <util/fixed_size_matrix.hpp>
#include <util/constants.h>
#include <util/clamp.h>
#include <cmath>
#include <cassert>
#include <vector>

using TransformationMatrix3d = sil::FixedSizeMatrix<float, 4, 4>;

namespace sil {
namespace transformations {

namespace detail {
static TransformationMatrix3d rotate_x(float phi) {
  TransformationMatrix3d Rx = {
      1.0f, 0.0f,      0.0f,      0.0f, 0.0f, cosf(phi), -sinf(phi), 0.0f,
      0.0f, sinf(phi), cosf(phi), 0.0f, 0.0f, 0.0f,      0.0f,       1.0f};
  return Rx;
}

static TransformationMatrix3d rotate_y(float phi) {
  TransformationMatrix3d Ry = {
      cosf(phi),  0.0f, sinf(phi), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      -sinf(phi), 0.0f, cosf(phi), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
  return Ry;
}

static TransformationMatrix3d rotate_z(float phi) {
  TransformationMatrix3d Rz = {
      cosf(phi), -sinf(phi), 0.0f, 0.0f, sinf(phi), cosf(phi), 0.0f, 0.0f,
      0.0f,      0.0f,       1.0f, 0.0f, 0.0f,      0.0f,      0.0f, 1.0f};
  return Rz;
}
}

static TransformationMatrix3d rotate3d(float Rx, float Ry, float Rz) {
  return detail::rotate_z(Rz) * detail::rotate_y(Ry) * detail::rotate_x(Rx);
}

static TransformationMatrix3d translate3d(float Tx, float Ty, float Tz) {
  TransformationMatrix3d T = {1.0f, 0.0f, 0.0f, Tx, 0.0f, 1.0f, 0.0f, Ty,
                              0.0f, 0.0f, 1.0f, Tz, 0.0f, 0.0f, 0.0f, 1.0f};
  return T;
}
static TransformationMatrix3d scale3d(float s) {
  TransformationMatrix3d S = {s,    0.0f, 0.0f, 0.0f, 0.0f, s,    0.0f, 0.0f,
                              0.0f, 0.0f, s,    0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
  return S;
}

static TransformationMatrix3d make_transform3d(float Tx = 0.0f, float Ty = 0.0f,
                                               float Tz = 0.0f, float Rx = 0.0f,
                                               float Ry = 0.0f, float Rz = 0.0f,
                                               float s = 1.0f) {
  return translate3d(Tx, Ty, Tz) * scale3d(s) * rotate3d(Rx, Ry, Rz);
}

static sil::Vec3f transform3d(sil::Vec3f v, TransformationMatrix3d const &m) {
  return sil::Vec3f{m.element[0][0] * v[0] + m.element[0][1] * v[1] +
                        m.element[0][2] * v[2] + m.element[0][3],
                    m.element[1][0] * v[0] + m.element[1][1] * v[1] +
                        m.element[1][2] * v[2] + m.element[1][3],
                    m.element[2][0] * v[0] + m.element[2][1] * v[1] +
                        m.element[2][2] * v[2] + m.element[2][3]};
}

static std::vector<sil::Vec3f> transform3d(std::vector<sil::Vec3f> const &v,
                                           TransformationMatrix3d const &m) {
  std::vector<sil::Vec3f> result(v.size());
  for (size_t i = 0; i < v.size(); i++) result[i] = transform3d(v[i], m);
  return result;
}

// Returns transformation matrix of an object given camera view vector.
// Rotation around z axis cannot be determined (here we assume it is 0), because
// system is underdetermined.
static TransformationMatrix3d direction_vector_to_transformation_matrix(
    sil::Vec3f v) {
  v = normalize<float>(v);

  float Rx = -std::asin(v[1]);
  float Ry = 0;

  if (std::cos(Rx) != 0)
    Ry = std::asin(clamp(v[0] / std::cos(Rx), -1.0f, 1.0f));

  if (v[2] < -1e-6) Ry = constants::pi<float>() - Ry;

  auto H = make_transform3d(0, 0, 0, Rx, Ry, 0, 1);

  assert(std::abs(std::cos(Rx) * std::cos(Ry) - v[2]) < 1e-2);
  assert(norm(transform3d(sil::Vec3f{0, 0, 1}, H.inverse()) - (v)) < 1e-2f);

  return H.inverse();
}

static sil::Vec3f transformation_matrix_to_direction_vector(
    TransformationMatrix3d H) {
  return transform3d(sil::Vec3f{0, 0, 1}, H.inverse());
}

static TransformationMatrix3d cancel_translation(TransformationMatrix3d m) {
  m.element[0][3] = 0;
  m.element[1][3] = 0;
  m.element[2][3] = 0;
  return m;
}
}
}

#endif
