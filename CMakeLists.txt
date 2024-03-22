cmake_minimum_required(VERSION 3.27)
cmake_policy(VERSION 3.27)
project(chip_8)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ProjectHelpers)

option(WITH_TESTS "Enable building and running of tests" FALSE)

find_package(SDL2 REQUIRED)

add_subdirectory(vm)

if (WITH_TESTS)
  include(FetchContent)
  FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG 8ac8190e494a381072c89f5e161b92a08d98b37b # v3.5.3
  )
  FetchContent_MakeAvailable(Catch2)

  enable_testing()
  add_subdirectory(test)
endif ()
