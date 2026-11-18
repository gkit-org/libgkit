set (GKIT_GRAPHIC "gkit_graphic")

set (GRAPHIC_SRC
    "./application.cpp"
    "./renderer.cpp"
    "./sprite_pass.cpp"
    "./window.cpp"
    "./render_object.cpp"
    "./render_queue.cpp"

    # Backend implementations (one subdirectory per graphics API)
    "./backend/opengl/device.cpp"
    "./backend/opengl/frame_buffer.cpp"
    "./backend/opengl/index_buffer.cpp"
    "./backend/opengl/render_buffer.cpp"
    "./backend/opengl/shader.cpp"
    "./backend/opengl/state_manager.cpp"
    "./backend/opengl/texture.cpp"
    "./backend/opengl/vertex_array.cpp"
    "./backend/opengl/vertex_buffer.cpp"
    "./backend/opengl/config.cpp"
)

add_library(${GKIT_GRAPHIC} OBJECT ${GRAPHIC_SRC})

target_include_directories(${GKIT_GRAPHIC} PRIVATE ${GKIT_SOURCE_DIR}/include)     # include for header file
target_include_directories(${GKIT_GRAPHIC} PRIVATE ${GKIT_SOURCE_DIR}/src)         # top include for source file
target_include_directories(${GKIT_GRAPHIC} PRIVATE ${GKIT_SOURCE_DIR}/third_party/glad_generated/include)  # glad generated headers

target_link_libraries(${GKIT_GRAPHIC} PRIVATE glad_gl SDL3::SDL3-static)
