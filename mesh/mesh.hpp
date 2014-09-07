#ifndef MESH_HPP_
#define MESH_HPP_

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyConnectivity.hh>
#include <OpenMesh/Core/Mesh/FinalMeshItemsT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/SubdividerT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>

#include <util/fixed_size_matrix.hpp>
#include <transformations/transformations3d.hpp>
#include <mesh/object_pose.hpp>

struct MeshTraits : public OpenMesh::DefaultTraits {
  FaceTraits {
    bool visible;
  };
};

typedef OpenMesh::TriMesh_ArrayKernelT<MeshTraits> Mesh;

static OpenMesh::Vec3f compute_centroid(Mesh const&);
static void transform_mesh(Mesh&, TransformationMatrix3d);

// align centroid with coordinate axes origin?
static Mesh read_mesh(std::string filename, bool align_centroid = true) {

  Mesh mesh;
  mesh.request_face_normals();
  mesh.request_vertex_colors();
  mesh.request_face_colors();

  OpenMesh::IO::Options opt;
  opt += OpenMesh::IO::Options::FaceNormal;
  opt += OpenMesh::IO::Options::FaceColor;
  opt += OpenMesh::IO::Options::VertexColor;
  opt += OpenMesh::IO::Options::VertexNormal;
  opt += OpenMesh::IO::Options::Binary;
  OpenMesh::IO::read_mesh(mesh, filename, opt);
  mesh.request_vertex_colors();
  mesh.request_face_colors();
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

static inline std::vector<OpenMesh::Vec3f> get_vertices(Mesh& mesh) {
  std::vector<OpenMesh::Vec3f> vertices;

  for (const auto& vertex : mesh.vertices()) {
    vertices.push_back(mesh.point(vertex));
  }
  return vertices;
}

static void subdivide_mesh(Mesh& mesh, int nsubdivisions = 1) {
  OpenMesh::Subdivider::Uniform::LoopT<Mesh, float> subdivider;
  subdivider.attach(mesh);
  subdivider(nsubdivisions);
  subdivider.detach();
  mesh.update_normals();
}

static size_t num_of_visible_faces(Mesh const& mesh) {
  size_t nvisible = 0;

  for (const auto& face : mesh.faces())
    if (mesh.data(face).visible) ++nvisible;

  return nvisible;
}

static OpenMesh::Vec3f compute_centroid(Mesh const& mesh) {
  OpenMesh::Vec3f centroid = {0.0f, 0.0f, 0.0f};

  for (const auto& vertex : mesh.vertices()) {
    centroid += mesh.point(vertex);
  }
  return centroid / static_cast<float>(mesh.n_vertices());
}

namespace detail {
static OpenMesh::Vec3f transform3d(OpenMesh::Vec3f const& v,
                                   TransformationMatrix3d const& H) {
  return {H.element[0][0] * v[0] + H.element[0][1] * v[1] +
              H.element[0][2] * v[2] + H.element[0][3],
          H.element[1][0] * v[0] + H.element[1][1] * v[1] +
              H.element[1][2] * v[2] + H.element[1][3],
          H.element[2][0] * v[0] + H.element[2][1] * v[1] +
              H.element[2][2] * v[2] + H.element[2][3]};
}
}

static OpenMesh::Vec3f pose_to_direction_vector(Pose pose) {
  auto H = sil::transformations::make_transform3d(0, 0, 0, pose.Rx, pose.Ry,
                                                  pose.Rz, 1);
  return detail::transform3d(OpenMesh::Vec3f{0, 0, 1}, H.inverse());
}

static void transform_mesh(Mesh& mesh,
                           TransformationMatrix3d transformation_matrix) {

  for (auto& vertex : mesh.vertices()) {
    mesh.set_point(
        vertex, detail::transform3d(mesh.point(vertex), transformation_matrix));
  }
}

static std::vector<std::tuple<sil::Vec3f, sil::Vec3f>>
get_visible_normal_vectors(Mesh const& mesh) {

  std::vector<std::tuple<sil::Vec3f, sil::Vec3f>> points_with_normals;

  for (const auto& face : mesh.faces()) {
    if (!mesh.data(face).visible) continue;

    auto fv_it = mesh.cfv_begin(face);
    auto fv_end = mesh.cfv_end(face);

    auto normal = mesh.normal(face);
    OpenMesh::Vec3f face_center = {0.0f, 0.0f, 0.0f};

    for (; fv_it != fv_end; fv_it++) face_center += mesh.point(*fv_it);
    face_center /= 3.0f;

    points_with_normals.emplace_back(std::make_tuple(
        sil::Vec3f{face_center[0], face_center[1], face_center[2]},
        sil::Vec3f{normal[0], normal[1], normal[2]}));
  }
  return points_with_normals;
}

#endif
