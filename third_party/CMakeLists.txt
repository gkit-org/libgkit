set (LIBRARY_OUTPUT_PATH ${BIN_FOLDER}/third_party)
message(${CMAKE_BINARY_DIR})

# build sdl (build for static lib)
set (SDL_STATIC ON)
set (SDL_SHARED ON)
set (SDL_BUILD_TESTS OFF)
set (SDL_INSTALL OFF)
set (SDL_TEST_LIBRARY OFF)
set (SDL_TESTS OFF)
add_subdirectory(sdl)

# buidld sdl_image
set (SDLIMAGE_INSTALL OFF)
set (SDLIMAGE_BUILD_SHARED_LIBS OFF)
set (SDLIMAGE_TESTS OFF)
add_subdirectory(sdl_image)