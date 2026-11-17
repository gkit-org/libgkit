# C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# compiler settings
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Project settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Global variable
set(GKIT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
if(NOT BIN_FOLDER)
    set(BIN_FOLDER "${CMAKE_BINARY_DIR}/bin")
endif()
set(LIBRARY_OUTPUT_PATH ${BIN_FOLDER})

# Add third party include paths
include_directories(${GKIT_SOURCE_DIR}/third_party/sdl/include)
include_directories(${GKIT_SOURCE_DIR}/third_party/sdl_image/include)

# Link third party libraries
link_directories(${BIN_FOLDER}/third_party)
