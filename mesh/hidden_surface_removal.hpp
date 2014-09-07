#ifndef HIDDEN_SURFACE_REMOVAL_HPP_
#define HIDDEN_SURFACE_REMOVAL_HPP_

#include <vector>

#include <bsp/mesh.hpp>

static void hidden_surface_removal(Mesh &mesh, OpenMesh::Vec3f view) {

  mesh.update_normals();
  for (const auto& face : mesh.faces())  {
    const auto n0 = mesh.normal(face);

    if (dot(n0, view) < 1e-3f) {
      mesh.data(face).visible = false;
    } else {
      mesh.data(face).visible = true;
    }
  }
}

#endif
