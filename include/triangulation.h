#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include <array>
#include <nlohmann/json.hpp>
#include <vector>

namespace triangulation {

using Point = std::array<double, 2>;
using Polygon = std::vector<Point>;
using Triangles = std::vector<Polygon>;

// Helper functions to access Point elements like .x and .y
inline double &x(Point &p) { return p[0]; }
inline double &y(Point &p) { return p[1]; }
inline const double &x(const Point &p) { return p[0]; }
inline const double &y(const Point &p) { return p[1]; }

/**
 * @brief A class to perform polygon triangulation.
 */
class Triangulator {
public:
  /**
   * @brief Triangulates a 2D polygon.
   *
   * @param polygon A vector of points representing the vertices of the polygon.
   * @return A vector of triangles, where each triangle is a vector of 3
   * points.
   */
  Triangles triangulate(const Polygon &polygon) const;
};

/**
 * @brief Parses a JSON object into a Polygon.
 *
 * @param json_points A nlohmann::json object containing an array of points.
 * @return A Polygon object.
 */
Polygon points_from_json(const nlohmann::json &json_points);

/**
 * @brief Serializes a vector of triangles into a JSON object.
 *
 * @param triangles The triangles to serialize.
 * @return A nlohmann::json object.
 */
nlohmann::json triangles_to_json(const Triangles &triangles);

} // namespace triangulation

#endif // TRIANGULATION_H