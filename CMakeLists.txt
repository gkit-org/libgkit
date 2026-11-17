cmake_minimum_required(VERSION 3.20)

project(gkit C CXX)

include(cmake/common.cmake)
include(cmake/options.cmake)

check_options_available()

# Third party dependencices build
add_subdirectory(third_party)

# Main library build
add_subdirectory(src)

# Test program build
if (GKIT_TEST_BUILD)
    if(NOT GKIT_STATIC_BUILD)
        message(FATAL_ERROR "Test build must enable with GKIT_STATIC_BUILD")
    endif()

    enable_testing()
    add_subdirectory(${GKIT_SOURCE_DIR}/test)
endif()

