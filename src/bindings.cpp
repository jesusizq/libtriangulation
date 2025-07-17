#include "triangulation.h"
#include <emscripten/bind.h>

using namespace emscripten;
using namespace triangulation;

namespace {
double get_x(const Point &p) { return p[0]; }
void set_x(Point &p, double val) { p[0] = val; }
double get_y(const Point &p) { return p[1]; }
void set_y(Point &p, double val) { p[1] = val; }
} // namespace

EMSCRIPTEN_BINDINGS(libtriangulation_module) {
  value_object<Point>("Point")
      .field("x", &get_x, &set_x)
      .field("y", &get_y, &set_y);

  register_vector<Point>("Polygon");
  register_vector<std::uint32_t>("Indices");

  class_<Triangulator>("Triangulator")
      .constructor<>()
      .function("triangulate",
                static_cast<Indices (Triangulator::*)(const Polygon &) const>(
                    &Triangulator::triangulate));
}