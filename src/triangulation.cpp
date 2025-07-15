#include "triangulation.h"

#include <mapbox/earcut.hpp>
#include <vector>

namespace triangulation {

Indices Triangulator::triangulate(const Polygon &polygon) const {
  std::vector<Polygon> polygons;
  polygons.push_back(polygon);

  // Run the triangulation
  const auto indices{mapbox::earcut(polygons)};
  return indices;
}

} // namespace triangulation