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

# Policy to allow FetchContent_Populate for compatibility with older deps
if(POLICY CMP0169)
  cmake_policy(SET CMP0169 OLD)
endif()

# --- Fetch dependencies ---
include(FetchContent)

# nlohmann_json
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_GetProperties(nlohmann_json)
if(NOT nlohmann_json_POPULATED)
  FetchContent_Populate(nlohmann_json)
endif()
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${nlohmann_json_SOURCE_DIR}/include)

# earcut.hpp
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
target_link_libraries(libtriangulation PUBLIC nlohmann_json earcut)

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

