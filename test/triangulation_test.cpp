#include "triangulation.h"
#include <gtest/gtest.h>
#include <set>

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
  const auto triangles{triangulator.triangulate(square_polygon)};
  ASSERT_EQ(triangles.size(), 2);

  auto t1 = get_canonical_triangle(triangles[0]);
  auto t2 = get_canonical_triangle(triangles[1]);

  // Define the two expected triangles canonically
  auto expected1 = get_canonical_triangle({{0, 0}, {1, 0}, {1, 1}});
  auto expected2 = get_canonical_triangle({{0, 0}, {1, 1}, {0, 1}});

  // The set of generated triangles must match the set of expected triangles
  bool valid_triangulation = (t1 == expected1 && t2 == expected2) ||
                             (t1 == expected2 && t2 == expected1);

  EXPECT_TRUE(valid_triangulation);
}