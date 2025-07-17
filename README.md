# libtriangulation

A C++ library for 2D polygon triangulation with **advanced self-intersection handling** and WebAssembly (WASM) support.

This library takes a list of 2D points defining a polygon and returns triangulation indices that can be used for rendering or further processing. It automatically detects and resolves self-intersections using Clipper2, then applies optimal triangulation algorithms. The library supports both native C++ compilation and WebAssembly compilation for use in web browsers.

## Features

- **Self-Intersection Detection & Resolution**: Automatically handles complex self-intersecting polygons
- **Dual Algorithm Approach**:
  - **Clipper2** for intersection resolution
  - **Earcut** for high-performance triangulation of simple polygons
- **WebAssembly Support**: Full WASM compilation with JavaScript bindings
- **Comprehensive Testing**: Extensive test suite including complex polygon cases
- **Easy Integration**: Header-only dependencies with automatic CMake fetching

## Algorithm Architecture

### 1. **Input Analysis**

The library first analyzes input polygons for self-intersections using a robust line-segment intersection algorithm.

### 2. **Intersection Resolution (Clipper2)**

For self-intersecting polygons, the library uses [Clipper2](https://github.com/AngusJohnson/Clipper2) to:

- Resolve all intersection points
- Split the polygon into separate non-overlapping regions
- Maintain proper polygon topology

### 3. **Optimized Triangulation (Earcut)**

Each resolved region is triangulated using [mapbox/earcut.hpp](https://github.com/mapbox/earcut.hpp):

- High-performance ear clipping algorithm
- Handles simple polygons without holes
- Produces consistent triangle winding

### 4. **Result Combination**

All triangulated regions are combined into:

- A unified vertex array (resolved geometry)
- A corresponding index array (triangle references)

## Dependencies

The library automatically fetches these dependencies at configure time:

- **[mapbox/earcut.hpp](https://github.com/mapbox/earcut.hpp)**: Core triangulation algorithm
- **[Clipper2](https://github.com/AngusJohnson/Clipper2)**: Self-intersection resolution
- **[GoogleTest](https://github.com/google/googletest)**: Unit testing framework (when testing enabled)

## How to Build and Test

### Native C++ Build

You need to have `CMake` and a C++17 compiler installed.

```bash
mkdir build && cd build && cmake .. -DBUILD_TESTING=ON && make -j$(nproc) && ctest
```

### WebAssembly Build

To build for WebAssembly (for use in web browsers), you need the Emscripten SDK:

```bash
# Install Emscripten SDK first
# Then build with WASM support
mkdir build_wasm && cd build_wasm && emcmake cmake .. -DBUILD_WASM=ON && make -j$(nproc)
```

This generates:

- `libtriangulation.js` - JavaScript wrapper with embedded or separate WASM
- `libtriangulation.wasm` - WebAssembly binary

The WASM build uses Emscripten's embind to provide a JavaScript API that mirrors the C++ interface.

## Algorithm Design Rationale

### Why Ear Clipping + Clipper2?

**Ear Clipping (mapbox/earcut.hpp):**

- **High Performance**: Extremely fast for simple polygons
- **Lightweight**: Header-only library with minimal dependencies
- **Proven Reliability**: Battle-tested in production applications
- **Optimal for Simple Cases**: Perfect for non-self-intersecting polygons

**Clipper2 Integration:**

- **Handles Complex Cases**: Resolves self-intersections that would break ear clipping
- **Robust Geometry Processing**: Professional-grade computational geometry
- **Maintains Topology**: Preserves proper polygon structure during resolution
- **Future-Proof**: Extensible for additional boolean operations

### Alternative Approaches Considered

**Delaunay Triangulation:**

- **Pros**: Higher quality triangles, better for numerical simulations
- **Cons**: More complex, slower for simple cases, overkill for visualization
- **Verdict**: Unnecessary complexity for this use case

**Constrained Delaunay:**

- **Pros**: Handles arbitrary constraints and holes
- **Cons**: Significantly more complex implementation
- **Verdict**: Would require major architectural changes

**Pure Clipper2 Triangulation:**

- **Pros**: Unified approach for all cases
- **Cons**: Slower than earcut for simple polygons
- **Verdict**: Our hybrid approach gets the best of both worlds

The current **hybrid architecture** provides optimal performance for simple polygons while gracefully handling complex self-intersecting cases.
