cmake_minimum_required(VERSION 3.20)

project(gkit C CXX)

include(cmake/common.cmake)

# Third party dependencices build
add_subdirectory(third_party)

# build core
set (GKIT_OBJS $<TARGET_OBJECTS:gkit_core>)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/core)

# build c_abi
if (DEFINED EXPORT_C_ABI AND EXPORT_C_ABI EQUAL true)
    list (APPEND GKIT_OBJS $<TARGET_OBJECTS:gkit_c_abi>)
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/c_abi)
endif() # ENABLE_C_ABI

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(${CMAKE_SOURCE_DIR}/test)
endif()

add_library(${PROJECT_NAME} ${GKIT_OBJS})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-static)

add_library(${PROJECT_NAME}_static STATIC ${GKIT_OBJS})
target_link_libraries(${PROJECT_NAME}_static PRIVATE SDL3::SDL3-static)
