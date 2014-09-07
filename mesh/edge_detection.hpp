#ifndef JUMP_EDGE_DETECTION_HPP_
#define JUMP_EDGE_DETECTION_HPP_

#include <mesh/mesh.hpp>
#include <mesh/edge.hpp>
#include <util/wrap_angles.h>

#include <vector>

namespace detail {
static inline float angle(OpenMesh::Vec3f const &a, OpenMesh::Vec3f const &b) {
  assert(a.norm() != 0.0f);
  assert(b.norm() != 0.0f);
  return std::acos((dot(a, b) - 1e-6f) / (a.norm() * b.norm()));
}

static inline bool is_edge_visible(Mesh const &mesh, Mesh::EdgeHandle handle,
                                   float minimum_normal_angle_diff) {
  auto face0 = mesh.face_handle(mesh.halfedge_handle(handle, 0));
  auto face1 = mesh.face_handle(mesh.halfedge_handle(handle, 1));

  bool visible0 = (face0.idx() != -1) && mesh.data(face0).visible;
  bool visible1 = (face1.idx() != -1) && mesh.data(face1).visible;

  // None of the neighbouring faces is visible
  if (!visible0 && !visible1) return false;

  // Both faces are visible. Check if angle is above the threshold.
  if (visible0 == visible1) {
    auto n0 = mesh.normal(face0);
    auto n1 = mesh.normal(face1);
    if (wrap_pi(detail::angle(n0, n1)) < minimum_normal_angle_diff) {
      return false;
    }
  }

  return true;
}
}

static std::vector<Edge> edge_detection(Mesh const &mesh,
                                        float minimum_normal_angle_diff) {

  std::vector<Edge> visible_edges;
  visible_edges.reserve(mesh.n_edges());

  Mesh::EdgeIter e_it = mesh.edges_begin();
  Mesh::EdgeIter end_it = mesh.edges_end();

  for (; e_it != end_it; e_it++) {
    if (detail::is_edge_visible(mesh, *e_it, minimum_normal_angle_diff)) {
      auto point0 =
          mesh.point(mesh.to_vertex_handle(mesh.halfedge_handle(*e_it, 0)));
      auto point1 =
          mesh.point(mesh.from_vertex_handle(mesh.halfedge_handle(*e_it, 0)));

      visible_edges.emplace_back(
          std::make_pair(sil::Vec3f{point0[0], point0[1], point0[2]},
                         sil::Vec3f{point1[0], point1[1], point1[2]}));
    }
  }

  return visible_edges;
}

#endif
