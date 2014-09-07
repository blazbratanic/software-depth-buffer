#ifndef MESH_INTERFACE_HPP
#define MESH_INTERFACE_HPP

#include <mesh/object_pose.hpp>
#include <mesh/mesh.hpp>
#include <mesh/edge_detection.hpp>
#include <mesh/hidden_surface_removal.hpp>
#include <mesh/pointcloud.hpp>
#include <transformations/transformations3d.hpp>

#include <algorithm>

struct MeshInterface {

  MeshInterface()
  {
	  isOpened = false;
  }
  MeshInterface(char* filename) 
  { 
	  mesh_ = read_mesh(filename);
	  isOpened = true;
  }

  void openMesh(char* filename) 
  { 
	  mesh_ = read_mesh(filename); 
	  isOpened = true;
  }

  bool isOpened;

  OrientedPointCloud get_contour(float* pose_ptr, float step_size = 1.0f) {

    Pose pose;
    std::copy_n(pose_ptr, 6, pose.flatten);

    float minimum_normal_diff = 1.0f;  // radians
    float point_cloud_step_size = step_size / pose.Tz;

    hidden_surface_removal(mesh_, pose_to_direction_vector(pose));
    auto visible_edges = edge_detection(mesh_, minimum_normal_diff);
    auto oriented_point_cloud =
        generate_oriented_point_cloud(visible_edges, point_cloud_step_size);

    auto H = sil::transformations::make_transform3d(
        pose.Tx, pose.Ty, 0, pose.Rx, pose.Ry, pose.Rz, pose.Tz);
    oriented_point_cloud =
        sil::transformations::transform3d(oriented_point_cloud, H);

	return oriented_point_cloud;
  }

 private:
  Mesh mesh_;
};

#endif
