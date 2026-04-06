cmake_minimum_required(VERSION 3.20)

project(gkit C CXX)

include(cmake/common.cmake)

# Third party dependencices build
add_subdirectory(third_party)

# Main library build
add_subdirectory(src)

# Test program build
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(${CMAKE_SOURCE_DIR}/test)
endif()

