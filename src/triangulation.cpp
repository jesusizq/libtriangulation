#include "triangulation.h"

#include <algorithm>
#include <clipper2/clipper.h>
#include <cmath>
#include <iostream>
#include <mapbox/earcut.hpp>
#include <vector>

namespace triangulation {

Indices Triangulator::triangulate(const Polygon &polygon) const {
  Polygon resolvedVertices;
  return triangulate(polygon, resolvedVertices);
}

Indices Triangulator::triangulate(const Polygon &polygon,
                                  Polygon &outVertices) const {
  if (polygon.size() < 3) {
    outVertices = polygon;
    return {}; // Can't triangulate with less than 3 points
  }

  // Check if the polygon has self-intersections
  if (!hasSelfIntersections(polygon)) {
    // Simple polygon, use earcut directly
    outVertices = polygon;
    return triangulateSimple(polygon);
  }

  // Self-intersecting polygon, resolve intersections first
  std::vector<Polygon> simplePolygons = resolveSelfIntersections(polygon);

  if (simplePolygons.empty()) {
    outVertices = polygon;
    return {};
  }

  // Create a combined vertex list from all resolved polygons
  outVertices.clear();
  Indices allIndices;

  // Triangulate each simple polygon and combine results
  std::uint32_t currentVertexOffset = 0;

  for (const auto &simplePolygon : simplePolygons) {
    // Add vertices to the combined list
    for (const auto &vertex : simplePolygon) {
      outVertices.push_back(vertex);
    }

    // Triangulate this simple polygon
    Indices simpleIndices = triangulateSimple(simplePolygon);

    // Adjust indices to account for the vertex offset and add to combined
    // result
    for (std::uint32_t idx : simpleIndices) {
      allIndices.push_back(idx + currentVertexOffset);
    }

    currentVertexOffset += simplePolygon.size();
  }

  return allIndices;
}

bool Triangulator::hasSelfIntersections(const Polygon &polygon) const {
  if (polygon.size() < 4) {
    return false; // Need at least 4 points to have self-intersections
  }

  // Check each pair of non-adjacent edges for intersection
  for (size_t i = 0; i < polygon.size(); ++i) {
    size_t next_i = (i + 1) % polygon.size();

    for (size_t j = i + 2; j < polygon.size(); ++j) {
      // Skip adjacent edges and the edge connecting last to first
      if (j == (i + polygon.size() - 1) % polygon.size()) {
        continue;
      }

      size_t next_j = (j + 1) % polygon.size();

      // Check if edges (i, next_i) and (j, next_j) intersect
      const Point &p1 = polygon[i];
      const Point &p2 = polygon[next_i];
      const Point &p3 = polygon[j];
      const Point &p4 = polygon[next_j];

      // Line intersection algorithm
      double denom =
          (x(p1) - x(p2)) * (y(p3) - y(p4)) - (y(p1) - y(p2)) * (x(p3) - x(p4));
      if (std::abs(denom) < 1e-10) {
        continue; // Lines are parallel
      }

      double t = ((x(p1) - x(p3)) * (y(p3) - y(p4)) -
                  (y(p1) - y(p3)) * (x(p3) - x(p4))) /
                 denom;
      double u = -((x(p1) - x(p2)) * (y(p1) - y(p3)) -
                   (y(p1) - y(p2)) * (x(p1) - x(p3))) /
                 denom;

      // Check if intersection point is within both line segments
      if (t > 0 && t < 1 && u > 0 && u < 1) {
        return true;
      }
    }
  }

  return false;
}

std::vector<Polygon>
Triangulator::resolveSelfIntersections(const Polygon &polygon) const {
  using namespace Clipper2Lib;

  // Convert our polygon to Clipper2 format
  PathD clipperPath;
  for (const auto &point : polygon) {
    clipperPath.push_back(PointD(x(point), y(point)));
  }

  PathsD clipperPaths = {clipperPath};

  // Use Clipper2 to resolve self-intersections by performing a union operation
  // This will split self-intersecting polygons into simple polygons
  ClipperD clipper;
  clipper.AddSubject(clipperPaths);

  PathsD solution;
  clipper.Execute(ClipType::Union, FillRule::NonZero, solution);

  // Convert back to our polygon format
  std::vector<Polygon> result;
  for (const auto &path : solution) {
    if (path.size() >= 3) {
      Polygon poly;
      for (const auto &point : path) {
        poly.push_back({point.x, point.y});
      }
      result.push_back(poly);
    }
  }

  // If Clipper2 didn't produce any valid polygons, fall back to original
  if (result.empty()) {
    result.push_back(polygon);
  }

  return result;
}

Indices Triangulator::triangulateSimple(const Polygon &polygon) const {
  std::vector<Polygon> polygons;
  polygons.push_back(polygon);

  // Run the triangulation using earcut
  return mapbox::earcut(polygons);
}

} // namespace triangulation