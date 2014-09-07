#ifndef LABEL_MESH_HPP
#define LABEL_MESH_HPP

#include <util/array2d.hpp>

bool pnpoly(Mesh const& mesh, Mesh::FaceIter face, float px, float py) {
  bool c = false;

  auto head_it = mesh.cfv_begin(face);
  auto tail_it = --mesh.cfv_end(face);

  auto fv_end = mesh.cfv_end(face);

  for (; head_it != fv_end; tail_it = head_it++) {
    if (((mesh.point(*head_it)[1] > py) != (mesh.point(*tail_it)[1] > py)) &&
        (px < (mesh.point(*tail_it)[0] - mesh.point(*head_it)[0]) *
                      (py - mesh.point(*head_it)[0]) /
                      (mesh.point(*tail_it)[1] - mesh.point(*head_it)[1]) +
                  mesh.point(*head_it)[0]))
      c = !c;
  }
  return c;
}

float get_height(Mesh const & mesh, Mesh::FaceIter face, float px, flaot py) {



}



int get_projected_mesh_label(Mesh& mesh, Array2dView<int> labeled_image,
                             Array2dView<float> depth_map) {
  sil::fill(labeled_image, -1);
  sil::fill(depth_map, 0.0f);

  for (const auto& face : mesh.faces()) {
    if (mesh.data(face).visible) {
    }
  }
}

#endif
