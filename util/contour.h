#ifndef CONTOUR_HPP_
#define CONTOUR_HPP_

#include <util/position.hpp>

#include <vector>
#include <utility>
#include <algorithm>

template <typename T>
using Contour = std::vector<ImPos<T>>;

template <typename T, typename U>
using OrientedContour = std::vector<OrientedImPos<T, U>>;

template <typename T>
Boundary<T> contour_boundary(Contour<T> const& contour) {

  ImPos<T> lu = {std::numeric_limits<T>::max(), std::numeric_limits<T>::max()};
  ImPos<T> rb = {std::numeric_limits<T>::lowest(),
                 std::numeric_limits<T>::lowest()};

  for (auto const& e : contour) {
    lu = min(lu, e);
    rb = max(rb, e);
  }

  return Boundary<T>{lu, rb};
}

template <typename T, typename U>
Boundary<T> contour_boundary(OrientedContour<T, U> const& contour) {

  ImPos<T> lu = {std::numeric_limits<T>::max(), std::numeric_limits<T>::max()};
  ImPos<T> rb = {std::numeric_limits<T>::lowest(),
                 std::numeric_limits<T>::lowest()};

  for (auto const& e : contour) {
    lu = min(lu, e.position);
    rb = max(rb, e.position);
  }

  return Boundary<T>{lu, rb};
}

#endif
