#ifndef POINTCLOUD_HPP_
#define POINTCLOUD_HPP_

#include <transformations/transformations3d.hpp>
#include <util/fixed_size_matrix.hpp>
#include <mesh/edge.hpp>

#include <vector>
#include <algorithm>
#include <tuple>

using PointCloud = std::vector<sil::Vec3f>;
using OrientedPointCloud = std::vector<std::tuple<sil::Vec3f, sil::Vec3f> >;

template <typename Point>
std::vector<Point> draw_line_between_two_points(
    Point position, Point end_position, typename Point::value_type step_size) {

  if (position == end_position) return std::vector<Point>();

  Point direction = end_position - position;
  direction = normalize(direction) * step_size;
  const size_t nsteps =
      static_cast<size_t>(distance(position, end_position) / step_size) + 1;

  std::vector<Point> contour;
  for (size_t i = 0; i < nsteps; i++) {
    contour.emplace_back(position);
    position = position + direction;
  }

  return contour;
}

static std::vector<PointOnEdge> generate_points_on_edge(std::vector<Edge> edges,
                                           float step_size) {
  std::vector<PointOnEdge> points_on_edge;

  for (const auto& edge : edges) {
    auto line_points =
        draw_line_between_two_points(edge.first, edge.second, step_size);

    std::transform(line_points.begin(), line_points.end(),
                   std::back_inserter(points_on_edge),
                   [&edge](sil::Vec3f const & p)
                       ->PointOnEdge { return std::make_pair(p, edge); });
  }
  return points_on_edge;
}

static OrientedPointCloud generate_oriented_point_cloud(std::vector<Edge> edges,
                                                        float step_size) {
  OrientedPointCloud oriented_point_cloud;

  for (const auto& edge : edges) {
    const auto& e0 = edge.first;
    const auto& e1 = edge.second;
    auto line_points = draw_line_between_two_points(e0, e1, step_size);

    const sil::Vec3f edge_normal =
        sil::Vec3f{e1[1] - e0[1], -(e1[0] - e0[0]), 0.0f};

    std::transform(line_points.begin(), line_points.end(),
                   std::back_inserter(oriented_point_cloud),
                   [edge_normal](sil::Vec3f const& p) {
      return std::make_tuple(p, edge_normal);
    });
  }
  return oriented_point_cloud;
}

namespace sil {
namespace transformations {
static OrientedPointCloud transform3d(
    OrientedPointCloud const& oriented_point_cloud,
    TransformationMatrix3d const& H) {
  OrientedPointCloud result(oriented_point_cloud.size());

  // Transformation matrix for normals
  const auto Hn = cancel_translation(H);

  std::transform(oriented_point_cloud.begin(), oriented_point_cloud.end(),
                 result.begin(),
                 [H, Hn](std::tuple<sil::Vec3f, sil::Vec3f> const& e) {
    return std::make_tuple(transform3d(std::get<0>(e), H),
                           transform3d(std::get<1>(e), Hn));
  });
  return result;
}
}
}

#endif
