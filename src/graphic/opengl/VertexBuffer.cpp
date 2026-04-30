#include "gkit/graphic/opengl/VertexBuffer.hpp"

#include <glad/gl.h>

gkit::graphic::opengl::buffer::VertexBuffer::VertexBuffer(const void* data, uint32_t size, bool Dynamic) {
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    if (!Dynamic){
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    } else {
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    }
    m_Size = size;
}

gkit::graphic::opengl::buffer::VertexBuffer::~VertexBuffer() {
    if(m_RendererID != 0) {
        glDeleteBuffers(1, &m_RendererID);
        m_RendererID = 0;
    }
}

gkit::graphic::opengl::buffer::VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
    : m_RendererID(other.m_RendererID)
    , m_Size(other.m_Size) {
    other.m_RendererID = 0;
}

auto gkit::graphic::opengl::buffer::VertexBuffer::operator=(VertexBuffer&& other) noexcept -> VertexBuffer& {
    if (this != &other) {
        if (m_RendererID != 0) {
            glDeleteBuffers(1, &m_RendererID);
        }
        m_RendererID = other.m_RendererID;
        m_Size = other.m_Size;
        other.m_RendererID = 0;
    }
    return *this;
}

auto gkit::graphic::opengl::buffer::VertexBuffer::update_data(const void* data, uint32_t size) -> void {
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    if (size == m_Size) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    } else {
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
        m_Size = size;
    }
}

auto gkit::graphic::opengl::buffer::VertexBuffer::update_sub_data(uint32_t offset, const void* data, uint32_t size) -> void {
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

auto gkit::graphic::opengl::buffer::VertexBuffer::bind() const -> void {
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

auto gkit::graphic::opengl::buffer::VertexBuffer::unbind() const -> void {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
