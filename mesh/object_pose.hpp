#ifndef pose_HPP_
#define pose_HPP_
#include <ostream>
#include <cmath>

#include <util/constants.h>
#include <util/near.h>
#include <util/wrap_angles.h>

#include <transformations/transformations3d.hpp>

struct Pose {
  union {
    double flatten[6];

    struct {
      double Tx;
      double Ty;
      double Tz;
      double Rx;
      double Ry;
      double Rz;
    };
  };
};

bool operator==(Pose const& a, Pose const& b) {
  return is_near(a.Tx, b.Tx, 1e-3) && is_near(a.Ty, b.Ty, 1e-3) &&
         is_near(a.Tz, b.Tz, 1e-3) && is_near(a.Rx, b.Rx, 1e-3) &&
         is_near(a.Ry, b.Ry, 1e-3) && is_near(a.Rz, b.Rz, 1e-3);
}

Pose transformation_matrix_to_pose(TransformationMatrix3d H, int option = 0) {
  Pose pose;
  pose.Tx = H.element[0][3];
  pose.Ty = H.element[1][3];
  pose.Tz = H.element[2][3];

  if (std::cos(H.element[2][0]) != 0) {
    pose.Ry;
    if (option == 0)
      pose.Ry = -std::asin(H.element[2][0]);
    else
      pose.Ry = constants::pi<float>() + std::asin(H.element[2][0]);
    pose.Ry = wrap_minus_pi_to_pi(pose.Ry);
    const float ry_cos = std::cos(pose.Ry);
    pose.Rx = std::atan2(H.element[2][1] * ry_cos, H.element[2][2] * ry_cos);
    pose.Rz = std::atan2(H.element[1][0] * ry_cos, H.element[0][0] * ry_cos);
  } else {
    pose.Rz = 0;
    if (is_near(H.element[2][0], -1.0f, 1e-3f)) {
      pose.Ry = constants::half_pi<float>();
      pose.Rx = pose.Rz + std::atan2(H.element[0][1], H.element[0][2]);
    } else {
      pose.Ry = -constants::half_pi<float>();
      pose.Rx = -pose.Rz + std::atan2(-H.element[0][1], -H.element[0][2]);
    }
  }
  return pose;
}

TransformationMatrix3d pose_to_transformation_matrix(Pose const& pose) {
  return sil::transformations::make_transform3d(pose.Tx, pose.Ty, pose.Tz,
                                                pose.Rx, pose.Ry, pose.Rz);
}

TransformationMatrix3d pose_to_rotation_matrix(Pose const& pose) {
  return sil::transformations::make_transform3d(0, 0, 0, pose.Rx, pose.Ry,
                                                pose.Rz);
}

TransformationMatrix3d pose_to_translation_matrix(Pose const& pose) {
  return sil::transformations::make_transform3d(pose.Tx, pose.Ty, pose.Tz);
}

std::ostream& operator<<(std::ostream& out, Pose const& pose) {
  out << pose.flatten[0] << " " << pose.flatten[1] << " " << pose.flatten[2]
      << " " << pose.flatten[3] << " " << pose.flatten[4] << " "
      << pose.flatten[5] << std::endl;
  return out;
}

#endif
