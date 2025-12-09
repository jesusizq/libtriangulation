# 📐 libtriangulation - A high-performance C++17 library for robust 2D polygon triangulation.

![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=flat&logo=c%2B%2B)
![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20WebAssembly-lightgrey)
![Google Test](https://img.shields.io/badge/Google%20Test-Enabled-blue?logo=googletest)
![License](https://img.shields.io/badge/license-MIT-green)

A high-performance C++17 library for **robust 2D polygon triangulation**. It handles complex edge cases—including self-intersections and holes—by combining **Clipper2** for boolean operations and **Earcut** for tessellation.

Designed for rendering pipelines, geometry processing, and WebAssembly applications.

## 🚀 Key Features

- **Robust Self-Intersection Handling**: Automatically detects and resolves complex topology errors using [Clipper2](https://github.com/AngusJohnson/Clipper2).
- **Hybrid Architecture**:
  - **Fast Path**: Direct O(n) ear-clipping for simple polygons.
  - **Robust Path**: Boolean resolution for complex/self-intersecting shapes.
- **WebAssembly Ready**: Full compilation support for running in browsers with JavaScript bindings.
- **Modern C++**: Header-only dependencies, type-safe `std::array` interface, and CMake `FetchContent` integration.

## 💻 Usage

The API is designed to be minimal and type-safe.

```cpp
#include "triangulation.h"
#include <vector>
#include <iostream>

int main() {
    using namespace triangulation;

    // 1. Define a polygon (Points are std::array<double, 2>)
    // Example: A self-intersecting "hourglass" shape
    Polygon polygon = {
        {0.0, 0.0}, {10.0, 10.0}, {10.0, 0.0}, {0.0, 10.0}
    };

    // 2. Create the triangulator
    Triangulator triangulator;

    // 3. Triangulate
    // We capture 'resolvedVertices' because self-intersections create new geometry points.
    Polygon resolvedVertices;
    Indices indices = triangulator.triangulate(polygon, resolvedVertices);

    // 4. Use results (e.g., for OpenGL/Vulkan index buffer)
    std::cout << "Generated " << indices.size() / 3 << " triangles from "
              << resolvedVertices.size() << " vertices.\n";

    return 0;
}
```

## 🏗 Architecture & Design Decisions

This library solves the common problem where standard triangulation algorithms crash on bad topology.

| Stage               | Technology      | Rationale                                                                                              |
| ------------------- | --------------- | ------------------------------------------------------------------------------------------------------ |
| **1. Analysis**     | **Input Check** | Checks for self-intersections. If clean, skips to Stage 3 (Zero-overhead for simple cases).            |
| **2. Resolution**   | **Clipper2**    | _The Heavy Lifter_. Resolves intersections and splits the polygon into valid, non-overlapping regions. |
| **3. Tessellation** | **Earcut.hpp**  | _The Speedster_. Applies high-performance ear clipping to the sanitized regions.                       |

### Why not just one algorithm?

- **Pure Earcut**: Fast but fails on self-intersections.
- **Pure Delaunay**: Overkill for rendering and often slower.
- **The Hybrid Approach**: We get the robustness of Clipper2 only when needed, maintaining the speed of Earcut for the 99% of simple cases.

## 🛠 Build & Install

We use modern CMake with automatic dependency fetching.

### Native C++ Build

Requires CMake and a C++17 compiler.

```bash
mkdir build && cd build
cmake .. -DBUILD_TESTING=ON
make -j$(nproc)
ctest --output-on-failure
```

### WebAssembly (WASM) Build

Requires Emscripten SDK. Generates `libtriangulation.js` and `.wasm` for web use.

```bash
mkdir build_wasm && cd build_wasm
emcmake cmake .. -DBUILD_WASM=ON
make -j$(nproc)
```

## 📦 Dependencies

The library automatically fetches these at configure time:

- **[mapbox/earcut.hpp](https://github.com/mapbox/earcut.hpp)**: Core triangulation.
- **[Clipper2](https://github.com/AngusJohnson/Clipper2)**: Intersection resolution.
- **[GoogleTest](https://github.com/google/googletest)**: Unit testing.

---
