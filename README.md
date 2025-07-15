# libtriangulation

A C++ library for 2D polygon triangulation with WebAssembly (WASM) support.

This library takes a list of 2D points defining a polygon and returns triangulation indices that can be used for rendering or further processing. It supports both native C++ compilation and WebAssembly compilation for use in web browsers.

## Dependencies

The library uses the following third-party libraries:

- [nlohmann/json](https://github.com/nlohmann/json): For parsing and serializing JSON data.
- [mapbox/earcut.hpp](https://github.com/mapbox/earcut.hpp): For the triangulation algorithm.
- [GoogleTest](https://github.com/google/googletest): For unit testing.

All dependencies are fetched automatically at configure time using CMake's `FetchContent`.

## How to Build and Test

### Native C++ Build

You need to have `CMake` and a C++17 compiler installed.

```bash
mkdir build && cd build && cmake .. && make && ctest
```

### WebAssembly Build

To build for WebAssembly (for use in web browsers), you need the Emscripten SDK:

```bash
# Install Emscripten SDK first
# Then build with WASM support
mkdir build_wasm && cd build_wasm
emcmake cmake .. -DBUILD_WASM=ON
make
```

This generates:

- `liblibtriangulation.js` - JavaScript wrapper with embedded WASM
- Optional: `liblibtriangulation.wasm` - Separate WASM binary

The WASM build uses Emscripten's embind to provide a JavaScript API that mirrors the C++ interface.

## Algorithm Choice: Ear Clipping (`mapbox/earcut.hpp`)

For this project, we chose the **Ear Clipping** algorithm as implemented in the `mapbox/earcut.hpp` library.

**Justification:**

- **Simplicity and Speed:** Ear clipping is relatively simple to implement and understand. It is very efficient for simple polygons without holes, which is the case for this project's requirements.
- **Lightweight:** `mapbox/earcut.hpp` is a header-only library, which makes it extremely easy to integrate into a CMake project with no complex build steps or dependencies.
- **Robustness:** The Mapbox implementation is a port of a widely-used JavaScript library and is well-tested and reliable.

### Comparison with Delaunay Triangulation

Another common triangulation method is **Delaunay Triangulation**.

- **Delaunay Triangulation:** This method produces triangles that are as close to equilateral as possible, maximizing the minimum angle of any triangle in the mesh. This "quality" is very desirable in many applications, such as finite element analysis and 3D graphics, as it avoids long, skinny triangles.

- **Trade-offs:**
  - **Complexity:** Delaunay algorithms are generally more complex to implement than ear clipping.
  - **Performance:** For simple polygon triangulation, ear clipping can be faster. Delaunay triangulation is more powerful and can handle more complex inputs (like point sets that are not forming a simple polygon), but this power is not needed for the current task.
  - **Output Quality:** Delaunay produces higher-quality meshes. However, for the simple visualization required by this project, the quality of the ear clipping output is perfectly sufficient.

Given the project's constraints and goals, the lightweight and fast nature of `mapbox/earcut.hpp` makes it a more pragmatic and suitable choice than a more complex Delaunay-based solution.
