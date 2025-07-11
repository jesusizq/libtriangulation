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

Polygon points_from_json(const nlohmann::json &json_points) {
  Polygon polygon;
  polygon.reserve(json_points.size());
  for (const auto &point : json_points) {
    polygon.emplace_back(Point{point[0].get<double>(), point[1].get<double>()});
  }
  return polygon;
}

nlohmann::json triangles_to_json(const Triangles &triangles) {
  auto json_triangles = nlohmann::json::array();

  for (const auto &triangle : triangles) {
    auto json_triangle = nlohmann::json::array();

    for (const auto &point : triangle) {
      json_triangle.push_back({x(point), y(point)});
    }
    json_triangles.push_back(json_triangle);
  }
  return json_triangles;
}

} // namespace triangulation