add_library(gkit_platform OBJECT register_types.cpp runtime.cpp window.cpp sdl_event_dispatcher.cpp)
target_include_directories(gkit_platform PRIVATE ${GKIT_SOURCE_DIR}/include ${GKIT_SOURCE_DIR}/src)
target_compile_features(gkit_platform PUBLIC cxx_std_20)
target_link_libraries(gkit_platform PRIVATE SDL3::SDL3-static)
