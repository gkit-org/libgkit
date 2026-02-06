# C standard
set(C_Standard 11)
set(C_Standard_REQUIRED ON)
set(C_EXTENSIONS OFF)

# C++ standard
set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

# Project settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin/${BIN_FOLDER})
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Add third party include paths
include_directories(${CMAKE_SOURCE_DIR}/third_party/sdl/include)

# Link third party libraries
link_directories(${CMAKE_SOURCE_DIR}/bin/${BIN_FOLDER})