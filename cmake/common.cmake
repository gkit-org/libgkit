# C standard
set(CMAKE_C_Standard 11)
set(CMAKE_C_Standard_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# compiler settings
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Project settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LIBRARY_OUTPUT_PATH ${BIN_FOLDER})

# Add third party include paths
include_directories(${CMAKE_SOURCE_DIR}/third_party/sdl/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/sdl_image/include)

# Link third party libraries

link_directories(${BIN_FOLDER}/third_party)
