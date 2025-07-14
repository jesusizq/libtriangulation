#include "triangulation.h"

#include <mapbox/earcut.hpp>
#include <vector>

namespace triangulation {

Triangles Triangulator::triangulate(const Polygon &polygon) const {
  std::vector<Polygon> polygons;
  polygons.push_back(polygon);

  // Run the triangulation
  const auto indices{mapbox::earcut(polygons)};

  // Convert the resulting indices to triangles
  Triangles triangles;
  triangles.reserve(indices.size() / 3);
  for (size_t i = 0; i < indices.size(); i += 3) {
    triangles.emplace_back(std::initializer_list<Point>{
        polygon[indices[i]], polygon[indices[i + 1]], polygon[indices[i + 2]]});
  }

  return triangles;
}

} // namespace triangulation