cmake_minimum_required(VERSION 3.15)
project(libtriangulation LANGUAGES CXX)

if(NOT PROJECT_VERSION)
  set(PROJECT_VERSION 0.0.1)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Project: ${PROJECT_NAME} Version: ${PROJECT_VERSION}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# Global options
option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" ON)
option(BUILD_TESTING "Enable test builds" OFF)
option(BUILD_WASM "Build for WebAssembly using Emscripten" OFF)

# Policy to allow FetchContent_Populate for compatibility with older deps
if(POLICY CMP0169)
  cmake_policy(SET CMP0169 OLD)
endif()

# --- Fetch dependencies ---
include(FetchContent)

FetchContent_Declare(
  earcut
  GIT_REPOSITORY https://github.com/mapbox/earcut.hpp.git
  GIT_TAG v2.2.4
)
FetchContent_GetProperties(earcut)
if(NOT earcut_POPULATED)
  FetchContent_Populate(earcut)
endif()
add_library(earcut INTERFACE)
target_include_directories(earcut INTERFACE ${earcut_SOURCE_DIR}/include)

# --- Library ---
add_library(libtriangulation)
target_include_directories(libtriangulation PUBLIC include)
target_sources(libtriangulation PRIVATE src/triangulation.cpp)

# Link dependencies
target_link_libraries(libtriangulation PUBLIC earcut)

# --- Emscripten specific ---
if(BUILD_WASM)
    message(STATUS "Building for Emscripten")
    
    # For WASM, create an executable instead of a library
    add_executable(libtriangulation_wasm 
        src/triangulation.cpp 
        src/bindings.cpp
    )
    
    target_include_directories(libtriangulation_wasm PUBLIC include)
    target_link_libraries(libtriangulation_wasm PUBLIC earcut)
    
    # Set proper output for WASM
    set_target_properties(libtriangulation_wasm PROPERTIES 
        OUTPUT_NAME libtriangulation
        SUFFIX .js
    )

    # Emscripten compile flags
    target_compile_options(libtriangulation_wasm PRIVATE --bind)
    
    # Emscripten link options for ES6 module
    target_link_options(libtriangulation_wasm PRIVATE
        --bind
        -sMODULARIZE=1
        -sEXPORT_NAME=createTriangulationModule
        -sENVIRONMENT=web
        -sEXPORT_ES6=1
        -sEXPORTED_FUNCTIONS=['_malloc','_free']
        -sALLOW_MEMORY_GROWTH=1
    )
endif()

# --- Tests ---
if(BUILD_TESTING)
  enable_testing()
  message(STATUS "Generating tests")

  # googletest
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
  )
  FetchContent_MakeAvailable(googletest)

  add_executable(tests test/triangulation_test.cpp)
  target_link_libraries(tests PRIVATE libtriangulation GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(tests) 
endif()

