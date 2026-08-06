set (GKIT_GRAPHIC "gkit_graphic")

set (GRAPHIC_SRC
    "./Renderer.cpp"
    "./RenderObject.cpp"
    "./RenderQueue.cpp"
    "./create_device.cpp"

    # Backend implementations (one subdirectory per graphics API)
    "./backend/opengl/Device.cpp"
    "./backend/opengl/FrameBuffer.cpp"
    "./backend/opengl/IndexBuffer.cpp"
    "./backend/opengl/RenderBuffer.cpp"
    "./backend/opengl/Shader.cpp"
    "./backend/opengl/StateManager.cpp"
    "./backend/opengl/Texture.cpp"
    "./backend/opengl/VertexArray.cpp"
    "./backend/opengl/VertexBuffer.cpp"
    "./backend/opengl/config.cpp"
)

add_library(${GKIT_GRAPHIC} OBJECT ${GRAPHIC_SRC})

target_include_directories(${GKIT_GRAPHIC} PRIVATE ${GKIT_SOURCE_DIR}/include)     # include for header file
target_include_directories(${GKIT_GRAPHIC} PRIVATE ${GKIT_SOURCE_DIR}/src)         # top include for source file
target_include_directories(${GKIT_GRAPHIC} PRIVATE ${GKIT_SOURCE_DIR}/third_party/glad_generated/include)  # glad generated headers

target_link_libraries(${GKIT_GRAPHIC} PRIVATE glad_gl)
