#ifndef LABEL_MESH_HPP
#define LABEL_MESH_HPP

#include <util/array2d.h>
#include <util/array2dview.h>
#include <util/array2dview_op.h>
#include <util/visualization.hpp>
#include <bsp/mesh.hpp>

#include <iostream>

namespace detail {
OpenMesh::Vec4f get_plane_parameters(OpenMesh::Vec3f const& p0,
                                     OpenMesh::Vec3f const& p1,
                                     OpenMesh::Vec3f const& p2) {
  auto p0p1 = p1 - p0;
  auto p0p2 = p2 - p0;
  auto np = p0p1 % p0p2;  // normal to the plane (a, b, c) in plane equation
  float d = p0 | np;
  return {np[0], np[1], np[2], d};
}

int orient2d(OpenMesh::Vec3f const& p0, OpenMesh::Vec3f const& p1,
             OpenMesh::Vec3f const& p2) {
  return (p1[0] - p0[0]) * (p2[1] - p0[1]) - (p1[1] - p0[1]) * (p2[0] - p0[0]);
}

void fill_triangle(Mesh const& mesh, Mesh::FaceHandle face_handle, int label,
                   Array2dView<int> labels, Array2dView<float> depth_map) {

  auto fv_it = mesh.cfv_begin(face_handle);
  OpenMesh::Vec3f p0 = mesh.point(*fv_it++);
  OpenMesh::Vec3f p1 = mesh.point(*fv_it++);
  OpenMesh::Vec3f p2 = mesh.point(*fv_it++);

  std::cout << p0 << p1 << p2 << std::endl;

  auto pp = get_plane_parameters(p0, p1, p2); // plane_parameters
  if (pp[2] < 1e-2f) return;

  int xmin = static_cast<int>(std::min({p0[0], p1[0], p2[0]}));
  int xmax = static_cast<int>(std::max({p0[0], p1[0], p2[0]}));
  int ymin = static_cast<int>(std::min({p0[1], p1[1], p2[1]}));
  int ymax = static_cast<int>(std::max({p0[1], p1[1], p2[1]}));

  // clipping
  xmin = std::max(xmin, 0);
  xmax = std::min(xmax, static_cast<int>(labels.GetSize1()) - 1);
  ymin = std::max(ymin, 0);
  ymax = std::min(ymax, static_cast<int>(labels.GetSize0()) - 1);
  if (xmax < 0 || ymax < 0) return;

  auto v0 = p0 - p1;
  auto v1 = p1 - p2;
  auto v2 = p2 - p0;

  int A01 = p0[1] - p1[1], B01 = p1[0] - p0[0];
  int A12 = p1[1] - p2[1], B12 = p2[0] - p1[0];
  int A20 = p2[1] - p0[1], B20 = p0[0] - p2[0];

  // Barycentric coordinates at minX/minY corner
  OpenMesh::Vec3f init_point = {static_cast<float>(xmin),
                                static_cast<float>(ymin), 0.0f};
  int w0_row = orient2d(p1, p2, init_point);
  int w1_row = orient2d(p2, p0, init_point);
  int w2_row = orient2d(p0, p1, init_point);

  std::cout << "xmin: " << xmin << ", xmax: " << xmax << ", ymin: " << ymin
            << ", ymax: " << ymax << std::endl;


  float dz = -pp[0] / pp[2];
  for (size_t row = ymin; row <= ymax; ++row) {
    float z = -(pp[0] * xmin + pp[1] * ymin + pp[3]) / pp[2];
    int w0 = w0_row;
    int w1 = w1_row;
    int w2 = w2_row;

    for (int col = xmin; col <= xmax; ++col) {

      // If p is on or inside all edges, render pixel.
      if (w0 >= 0 && w1 >= 0 && w2 >= 0 && z >= depth_map(row, col)) {
         std::cout << "Row: " << row << ", col: " << col << "depth: " << z <<
         std::endl;
        depth_map(row, col) = z;
        labels(row, col) = 1;
      }
      // One step to the right
      w0 += A12;
      w1 += A20;
      w2 += A01;
      z += dz;
    }

    // One row step
    w0_row += B12;
    w1_row += B20;
    w2_row += B01;
  }
}
}

int get_projected_mesh_label(Mesh& mesh, Array2dView<int> labeled_image,
                             Array2dView<float> depth_map) {
  sil::fill(labeled_image, 0);
  sil::fill(depth_map, 0.0f);
  int label = 0;

  for (const auto& face : mesh.faces()) {
    //if (mesh.data(face).visible) {
      detail::fill_triangle(mesh, face, label++, labeled_image, depth_map);
    //}
  }
}

int main() {
  auto mesh = read_mesh("model.stl");
  transform_mesh(mesh, sil::transformations::scale3d(1000));
  Array2d<float> depth_map(500, 500);
  Array2d<int> label(500, 500);
  get_projected_mesh_label(mesh, label, depth_map);
  sil::multiply(depth_map, 200.0f, depth_map);
  save_image(depth_map, "depth_map.png");
  save_image(label, "label.png");
  return 0;
}

#endif
