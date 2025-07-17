#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include <array>
#include <cstdint>
#include <vector>

namespace triangulation {

using Point = std::array<double, 2>;
using Polygon = std::vector<Point>;
using Indices = std::vector<std::uint32_t>;

// Helper functions to access Point elements like .x and .y
inline double &x(Point &p) { return p[0]; }
inline double &y(Point &p) { return p[1]; }
inline const double &x(const Point &p) { return p[0]; }
inline const double &y(const Point &p) { return p[1]; }

/**
 * @brief A class to perform polygon triangulation.
 *
 * This class can handle both simple and self-intersecting polygons.
 * For self-intersecting polygons, it first resolves the intersections
 * using Clipper2 and then triangulates the resulting simple polygons.
 */
class Triangulator {
public:
  /**
   * @brief Triangulates a 2D polygon.
   *
   * This method automatically detects and handles self-intersecting polygons.
   * For simple polygons, it uses earcut directly. For self-intersecting
   * polygons, it first resolves intersections using Clipper2 and then
   * triangulates each resulting simple polygon.
   *
   * @param polygon A vector of points representing the vertices of the polygon.
   * @return A vector of indices representing the triangles.
   */
  Indices triangulate(const Polygon &polygon) const;

  /**
   * @brief Triangulates a 2D polygon and returns both resolved vertices and
   * indices.
   *
   * This method is useful when dealing with self-intersecting polygons where
   * the resolved geometry may differ from the input polygon.
   *
   * @param polygon A vector of points representing the vertices of the polygon.
   * @param outVertices Output parameter that will contain the resolved
   * vertices.
   * @return A vector of indices representing the triangles (referencing
   * outVertices).
   */
  Indices triangulate(const Polygon &polygon, Polygon &outVertices) const;

private:
  /**
   * @brief Checks if a polygon is self-intersecting.
   *
   * @param polygon The polygon to check.
   * @return true if the polygon has self-intersections, false otherwise.
   */
  bool hasSelfIntersections(const Polygon &polygon) const;

  /**
   * @brief Resolves self-intersections in a polygon using Clipper2.
   *
   * @param polygon The self-intersecting polygon.
   * @return A vector of simple polygons that together represent the original
   * polygon.
   */
  std::vector<Polygon> resolveSelfIntersections(const Polygon &polygon) const;

  /**
   * @brief Triangulates a simple polygon using earcut.
   *
   * @param polygon A simple (non-self-intersecting) polygon.
   * @return A vector of indices representing the triangles.
   */
  Indices triangulateSimple(const Polygon &polygon) const;
};

} // namespace triangulation

#endif // TRIANGULATION_H