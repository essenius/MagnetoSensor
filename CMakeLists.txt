cmake_minimum_required(VERSION 3.14)

set(projectName MagnetoSensor)
set(projectTestName ${projectName}Test)
set(espMockName esp32-mock) # lower case is important for fetching the library

project(${projectName} VERSION 0.0.6 LANGUAGES CXX)

# We only want to enable testing and code coverage for the top level project
# On Windows, these are also only used when building a vcpkg package.
if ((CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR) AND NOT (WIN32))
  set(TOP_LEVEL ON)
else()
  set(TOP_LEVEL OFF)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/lib;${PROJECT_BINARY_DIR}/lib/cmake")

add_subdirectory(lib)
add_subdirectory(src)

if (TOP_LEVEL) 
  message(STATUS "Top level project - enabling tests")
  set(CODE_COVERAGE ON)
  include(code-coverage)
  add_code_coverage()
  enable_testing()
  add_subdirectory(test)
  target_code_coverage(${projectTestName} EXCLUDE build/_deps/googletest-src/* test/*) 
endif()
