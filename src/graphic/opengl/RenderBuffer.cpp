#include "gkit/graphic/opengl/RenderBuffer.hpp"

#include <glad/gl.h>

gkit::graphic::opengl::buffer::RenderBuffer::RenderBuffer(int width, int height)
{
	glGenRenderbuffers(1, &m_RendererID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	Unbind();
}

gkit::graphic::opengl::buffer::RenderBuffer::~RenderBuffer()
{
	if(m_RendererID != 0)
	{
		glDeleteRenderbuffers(1, &m_RendererID);
		m_RendererID = 0;
	}
}

auto gkit::graphic::opengl::buffer::RenderBuffer::Bind() const -> void
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
}
auto gkit::graphic::opengl::buffer::RenderBuffer::Unbind() const -> void
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}