#ifndef LABEL_MESH_HPP
#define LABEL_MESH_HPP
#include <util/array2dview.h>
#include <mesh/mesh.hpp>
#include <transformations/transformation_matrix.h>
#include <transformations/transformations3d.hpp>

void get_projected_depth_and_label(Mesh mesh, TransformationMatrix3d H,
                                   Array2dView<int> labeled_image,
                                   Array2dView<float> depth_map);

#endif
