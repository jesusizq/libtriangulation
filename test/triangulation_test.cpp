#include "triangulation.h"
#include <gtest/gtest.h>
#include <set>

// Helper to create a canonical representation of a triangle's points from
// indices
std::set<std::pair<double, double>>
get_canonical_triangle_from_indices(const triangulation::Polygon &polygon,
                                    const triangulation::Indices &indices,
                                    size_t triangle_index) {
  std::set<std::pair<double, double>> points;
  size_t base_idx = triangle_index * 3;
  for (size_t i = 0; i < 3; ++i) {
    const auto &p = polygon[indices[base_idx + i]];
    points.insert({triangulation::x(p), triangulation::y(p)});
  }
  return points;
}

// Helper to create a canonical representation of a triangle's points
std::set<std::pair<double, double>>
get_canonical_triangle(const triangulation::Polygon &triangle) {
  std::set<std::pair<double, double>> points;
  for (const auto &p : triangle) {
    points.insert({triangulation::x(p), triangulation::y(p)});
  }
  return points;
}

// Test fixture for triangulation tests
class TriangulationTest : public ::testing::Test {
protected:
  void SetUp() override {
    // A simple square polygon
    square_polygon = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  }

  triangulation::Polygon square_polygon;
};

TEST_F(TriangulationTest, TriangulateSquare) {
  triangulation::Triangulator triangulator;
  const auto indices{triangulator.triangulate(square_polygon)};

  // For a square, we expect 2 triangles (6 indices)
  ASSERT_EQ(indices.size(), 6);
  ASSERT_EQ(indices.size() % 3, 0); // Must be multiple of 3

  size_t num_triangles = indices.size() / 3;
  ASSERT_EQ(num_triangles, 2);

  // Verify all indices are valid
  for (const auto &idx : indices) {
    EXPECT_LT(idx, square_polygon.size());
  }

  auto t1 = get_canonical_triangle_from_indices(square_polygon, indices, 0);
  auto t2 = get_canonical_triangle_from_indices(square_polygon, indices, 1);

  // Define the two expected triangles canonically
  auto expected1 = get_canonical_triangle({{0, 0}, {1, 0}, {1, 1}});
  auto expected2 = get_canonical_triangle({{0, 0}, {1, 1}, {0, 1}});

  // The set of generated triangles must match the set of expected triangles
  bool valid_triangulation = (t1 == expected1 && t2 == expected2) ||
                             (t1 == expected2 && t2 == expected1);

  EXPECT_TRUE(valid_triangulation);
}

TEST_F(TriangulationTest, TriangulateComplexPolygon) {
  // Test with a more complex polygon (like points_4.json)
  triangulation::Polygon complex_polygon = {
      {-6.0, 6.0},
      {1.87341821193695, 3.77215147018433},
      {6.98734188079834, 5.29113864898682},
      {9.97468376159668, 4.8354434967041},
      {9.31645584106445, 1.59493684768677},
      {0.30379718542099, -1.08860731124878},
      {10.0253171920776, 0.126582384109497},
      {12.8101263046265, -5.84810161590576},
      {8.75949287414551, -6.40506315231323},
      {1.97468328475952, 7.67088651657104},
      {-6.0, -6.0},
      {-11.8987340927124, 0.177215337753296}};

  triangulation::Triangulator triangulator;
  const auto indices{triangulator.triangulate(complex_polygon)};

  // For complex polygons, earcut may produce fewer triangles than (n-2) due to
  // self-intersections or optimizations. We just verify the result is
  // reasonable.
  ASSERT_GT(indices.size(), 0);     // Must have some triangles
  ASSERT_EQ(indices.size() % 3, 0); // Must be multiple of 3

  // Should have at least a few triangles but not more than the theoretical
  // maximum
  size_t num_triangles = indices.size() / 3;
  size_t max_triangles = complex_polygon.size() - 2;
  EXPECT_GT(num_triangles, 0);
  EXPECT_LE(num_triangles, max_triangles);

  // Verify all indices are valid
  for (const auto &idx : indices) {
    EXPECT_LT(idx, complex_polygon.size());
  }
}