cmake_minimum_required(VERSION 3.20)

project(gkit)

# C++ standard
set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

# C standard
set(C_Standard 11)
set(C_Standard_REQUIRED ON)
set(C_EXTENSIONS OFF)

# Project settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/core/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/c_abi/include/)

# Third party libraries
find_package(SDL3 REQUIRED)

## Core sources ##
file(GLOB_RECURSE CORE_APPLICATION_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/application/*.cpp
)

file(GLOB_RECURSE CORE_GRAPHIC_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/graphic/*.cpp
)

file(GLOB_RECURSE CORE_INPUT_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/input/*.cpp
)

file(GLOB_RECURSE CORE_UTILS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/utils/*.cpp
)

set(CORE_SRC
    ${CORE_APPLICATION_SRC}
    ${CORE_GRAPHIC_SRC}
    ${CORE_INPUT_SRC}
    ${CORE_UTILS_SRC}
)

if (ENABLE_C_ABI)

    ## CABI sources ##
    file(GLOB_RECURSE CABI_APPLICATION_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_abi/application/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_abi/application/*.cpp
    )

    file(GLOB_RECURSE CABI_GRAPHIC_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_abi/graphic/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/c_abi/graphic/*.cpp
    )


    set(CABI_SRC
        ${CABI_APPLICATION_SRC}
        ${CABI_GRAPHIC_SRC}
    )

endif() # ENABLE_C_ABI


set(GKIT_SRC
    ${CORE_SRC}
    ${CABI_SRC}
)

add_library(gkit SHARED ${GKIT_SRC})
add_library(gkit_static STATIC ${GKIT_SRC})

target_link_libraries(gkit PRIVATE SDL3::SDL3)
target_link_libraries(gkit_static PRIVATE SDL3::SDL3)
