#include "mesh.hpp"
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyConnectivity.hh>
#include <OpenMesh/Core/Mesh/FinalMeshItemsT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/SubdividerT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>

#include <util/fixed_size_matrix.hpp>
#include <transformations/transformations3d.hpp>
#include <features/object_pose.hpp>

OpenMesh::Vec3f compute_centroid(Mesh const& mesh);
// align centroid with coordinate axes origin?
Mesh read_mesh(std::string filename, bool align_centroid) {

  Mesh mesh;
  mesh.request_face_normals();

  OpenMesh::IO::Options opt;
  opt += OpenMesh::IO::Options::FaceNormal;
  opt += OpenMesh::IO::Options::Binary;
  OpenMesh::IO::read_mesh(mesh, filename);
  mesh.update_normals();

  if (align_centroid) {
    auto mesh_centroid = compute_centroid(mesh);
    transform_mesh(
        mesh, sil::transformations::translate3d(
                  -mesh_centroid[0], -mesh_centroid[1], -mesh_centroid[2]));
  }
  mesh.update_normals();

  return mesh;
}

inline std::vector<OpenMesh::Vec3f> get_vertices(Mesh& mesh) {
  std::vector<OpenMesh::Vec3f> vertices;

  for (const auto& vertex : mesh.vertices()) {
    vertices.push_back(mesh.point(vertex));
  }
  return vertices;
}

void subdivide_mesh(Mesh& mesh, int nsubdivisions) {
  OpenMesh::Subdivider::Uniform::LoopT<Mesh, float> subdivider;
  subdivider.attach(mesh);
  subdivider(nsubdivisions);
  subdivider.detach();
  mesh.update_normals();
}

size_t num_of_visible_faces(Mesh const& mesh) {
  size_t nvisible = 0;

  for (const auto& face : mesh.faces())
    if (mesh.data(face).visible) ++nvisible;

  return nvisible;
}

OpenMesh::Vec3f compute_centroid(Mesh const& mesh) {
  OpenMesh::Vec3f centroid = {0.0f, 0.0f, 0.0f};

  for (const auto& vertex : mesh.vertices()) {
    centroid += mesh.point(vertex);
  }
  return centroid / static_cast<float>(mesh.n_vertices());
}

namespace detail {
OpenMesh::Vec3f transform3d(OpenMesh::Vec3f const& v,
                                   TransformationMatrix3d const& H) {
  return {H.element[0][0] * v[0] + H.element[0][1] * v[1] +
              H.element[0][2] * v[2] + H.element[0][3],
          H.element[1][0] * v[0] + H.element[1][1] * v[1] +
              H.element[1][2] * v[2] + H.element[1][3],
          H.element[2][0] * v[0] + H.element[2][1] * v[1] +
              H.element[2][2] * v[2] + H.element[2][3]};
}
}


void transform_mesh(Mesh& mesh,
                           TransformationMatrix3d transformation_matrix) {

  for (auto& vertex : mesh.vertices()) {
    mesh.set_point(
        vertex, detail::transform3d(mesh.point(vertex), transformation_matrix));
  }
}


