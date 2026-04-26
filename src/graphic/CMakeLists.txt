set (GKIT_GRAPHIC "gkit_graphic")

set (GRAPHIC_SRC
    "./Renderer.cpp"
    "./Shader.cpp"

    # OpenGL module
    "./opengl/FrameBuffer.cpp"
    "./opengl/IndexBuffer.cpp"
    "./opengl/RenderBuffer.cpp"
    "./opengl/StateManager.cpp"
    "./opengl/Texture.cpp"
    "./opengl/VertexArray.cpp"
    "./opengl/VertexBuffer.cpp"
    "./opengl/config.cpp"
)

add_library(${GKIT_GRAPHIC} OBJECT ${GRAPHIC_SRC})

target_include_directories(${GKIT_GRAPHIC} PRIVATE ${CMAKE_SOURCE_DIR}/include)     # include for header file
target_include_directories(${GKIT_GRAPHIC} PRIVATE ${CMAKE_SOURCE_DIR}/src)         # top include for source file
target_include_directories(${GKIT_GRAPHIC} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/glad_generated/include)  # glad generated headers

target_link_libraries(${GKIT_GRAPHIC} PRIVATE glad_gl)
