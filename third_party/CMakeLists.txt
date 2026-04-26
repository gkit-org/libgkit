set (LIBRARY_OUTPUT_PATH ${BIN_FOLDER}/third_party)

# build glad (generated sources into third_party/glad_generated folder)
add_subdirectory(glad/cmake)

glad_add_library(glad_gl STATIC
    API "gl:core=4.6"
    REPRODUCIBLE
    LOCATION "${CMAKE_SOURCE_DIR}/third_party/glad_generated"
)

target_include_directories(glad_gl INTERFACE
    ${CMAKE_SOURCE_DIR}/third_party/glad_generated/include
)

# build sdl (build for static lib)
set (SDL_STATIC ON)
set (SDL_SHARED OFF)
set (SDL_BUILD_SHARED_LIBS OFF)
set (SDL_STATIC_PIC ON)
set (SDL_BUILD_TESTS OFF)
set (SDL_INSTALL OFF)
set (SDL_TEST_LIBRARY OFF)
set (SDL_TESTS OFF)
add_subdirectory(sdl)

# build sdl_image
set (BUILD_SHARED_LIBS OFF)
set (SDLIMAGE_INSTALL OFF)
set (SDLIMAGE_BUILD_SHARED_LIBS OFF)
set (SDLIMAGE_TESTS OFF)
add_subdirectory(sdl_image EXCLUDE_FROM_ALL)