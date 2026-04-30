#include "gkit/graphic/opengl/RenderBuffer.hpp"

#include <glad/gl.h>

gkit::graphic::opengl::buffer::RenderBuffer::RenderBuffer(int width, int height) {
	glGenRenderbuffers(1, &m_RendererID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	unbind();
}

gkit::graphic::opengl::buffer::RenderBuffer::~RenderBuffer() {
	if(m_RendererID != 0) {
		glDeleteRenderbuffers(1, &m_RendererID);
		m_RendererID = 0;
	}
}

gkit::graphic::opengl::buffer::RenderBuffer::RenderBuffer(RenderBuffer&& other) noexcept
    : m_RendererID(other.m_RendererID) {
    other.m_RendererID = 0;
}

auto gkit::graphic::opengl::buffer::RenderBuffer::operator=(RenderBuffer&& other) noexcept -> RenderBuffer& {
    if (this != &other) {
        if (m_RendererID != 0) {
            glDeleteRenderbuffers(1, &m_RendererID);
        }
        m_RendererID = other.m_RendererID;
        other.m_RendererID = 0;
    }
    return *this;
}

auto gkit::graphic::opengl::buffer::RenderBuffer::bind() const -> void {
	glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
}

auto gkit::graphic::opengl::buffer::RenderBuffer::unbind() const -> void {
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}