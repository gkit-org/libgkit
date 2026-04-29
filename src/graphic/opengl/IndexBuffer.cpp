#include "gkit/graphic/opengl/IndexBuffer.hpp"

#include <glad/gl.h>

gkit::graphic::opengl::buffer::IndexBuffer::IndexBuffer(const uint32_t* data, uint32_t count)
    : m_Count(count) {
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
}
gkit::graphic::opengl::buffer::IndexBuffer::~IndexBuffer() {
    if(m_RendererID != 0) {
        glDeleteBuffers(1, &m_RendererID);
        m_RendererID = 0;
    }
}

gkit::graphic::opengl::buffer::IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
    : m_RendererID(other.m_RendererID)
    , m_Count(other.m_Count) {
    other.m_RendererID = 0;
}

auto gkit::graphic::opengl::buffer::IndexBuffer::operator=(IndexBuffer&& other) noexcept -> IndexBuffer& {
    if (this != &other) {
        if (m_RendererID != 0) {
            glDeleteBuffers(1, &m_RendererID);
        }
        m_RendererID = other.m_RendererID;
        m_Count = other.m_Count;
        other.m_RendererID = 0;
    }
    return *this;
}

auto gkit::graphic::opengl::buffer::IndexBuffer::Bind() const -> void {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

auto gkit::graphic::opengl::buffer::IndexBuffer::Unbind() const -> void {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}