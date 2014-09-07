#ifndef EDGE_HPP_
#define EDGE_HPP_
#include <utility>
#include <vector>
#include <transformations/transformations3d.hpp>

#include <util/fixed_size_matrix.hpp>

using Edge = std::pair<sil::Vec3f, sil::Vec3f>;
using PointOnEdge = std::pair<sil::Vec3f, Edge>;

static inline Edge transform3d(Edge const& edge,
                               TransformationMatrix3d transformation_matrix) {
  using namespace sil::transformations;
  return std::make_pair(transform3d(edge.first, transformation_matrix),
                        transform3d(edge.second, transformation_matrix));
}

static inline PointOnEdge transform3d(
    PointOnEdge const& edge, TransformationMatrix3d transformation_matrix) {
  using namespace sil::transformations;
  return std::make_pair(transform3d(edge.first, transformation_matrix),
                        transform3d(edge.second, transformation_matrix));
}

template <typename T>
static std::vector<T> transforme3d(
    std::vector<T> const& edges,
    TransformationMatrix3d transformation_matrix) {
  std::vector<T> result(edges.size());
  for (size_t i = 0; i < edges.size(); i++) {
    result[i] = transform3d(edges[i], transformation_matrix);
  }
  return result;
}

#endif
