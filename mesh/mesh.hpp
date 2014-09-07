#ifndef MESH_HPP_
#define MESH_HPP_

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
//#include <OpenMesh/Core/Mesh/PolyConnectivity.hh>
//#include <OpenMesh/Core/Mesh/FinalMeshItemsT.hh>

#include <util/fixed_size_matrix.hpp>
#include <transformations/transformations3d.hpp>

struct MeshTraits : public OpenMesh::DefaultTraits {
  FaceTraits {
    bool visible;
  };
};

typedef OpenMesh::TriMesh_ArrayKernelT<MeshTraits> Mesh;

void transform_mesh(Mesh&, TransformationMatrix3d);

// align centroid with coordinate axes origin?
Mesh read_mesh(std::string filename, bool align_centroid = true);

inline std::vector<OpenMesh::Vec3f> get_vertices(Mesh& mesh);

void subdivide_mesh(Mesh& mesh, int nsubdivisions = 1);

size_t num_of_visible_faces(Mesh const& mesh);

void transform_mesh(Mesh& mesh,
                           TransformationMatrix3d transformation_matrix);

#endif
