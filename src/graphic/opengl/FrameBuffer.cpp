#include "gkit/graphic/opengl/FrameBuffer.hpp"
#include "gkit/graphic/opengl/Texture.hpp"

#include <glad/gl.h>

#include <iostream>

gkit::graphic::opengl::buffer::FrameBuffer::FrameBuffer(int width, int height)
	: fb_width(width), fb_height(height) {
	glGenFramebuffers(1, &m_RendererID);
}

gkit::graphic::opengl::buffer::FrameBuffer::~FrameBuffer() {
	if(m_RendererID != 0) {
		glDeleteFramebuffers(1, &m_RendererID);
		m_RendererID = 0;
	}
}

gkit::graphic::opengl::buffer::FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
    : m_RendererID(other.m_RendererID)
    , fb_height(other.fb_height)
    , fb_width(other.fb_width)
    , leftX(other.leftX)
    , bottomY(other.bottomY) {
    other.m_RendererID = 0;
}

auto gkit::graphic::opengl::buffer::FrameBuffer::operator=(FrameBuffer&& other) noexcept -> FrameBuffer& {
    if (this != &other) {
        if (m_RendererID != 0) {
            glDeleteFramebuffers(1, &m_RendererID);
        }
        m_RendererID = other.m_RendererID;
        fb_height = other.fb_height;
        fb_width = other.fb_width;
        leftX = other.leftX;
        bottomY = other.bottomY;
        other.m_RendererID = 0;
    }
    return *this;
}

auto gkit::graphic::opengl::buffer::FrameBuffer::attach_color_texture(const gkit::graphic::opengl::Texture& texture, int slot) -> void {
	bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_2D, texture.get_render_id(), 0);
}

auto gkit::graphic::opengl::buffer::FrameBuffer::detach_color_texture(int slot) -> void {
	bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_2D, 0, 0);
}

auto gkit::graphic::opengl::buffer::FrameBuffer::attach_depth_stencil(const RenderBuffer& rbo) -> void {
	bind();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 
		GL_RENDERBUFFER, rbo.get_render_id());
}

auto gkit::graphic::opengl::buffer::FrameBuffer::detach_depth_stencil() -> void {
	bind();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
								GL_RENDERBUFFER, 0);
}

auto gkit::graphic::opengl::buffer::FrameBuffer::check() -> void {
	bind();
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "==================================================" << std::endl;
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		std::cout << "==================================================" << std::endl;
	} else {
		std::cout << "FRAMEBUFFER:: Framebuffer is complete!" << std::endl;
	}
	unbind();
}

auto gkit::graphic::opengl::buffer::FrameBuffer::set_viewport(int x, int y, int width, int height) -> void {
	glViewport(x, y, width, height);
}

auto gkit::graphic::opengl::buffer::FrameBuffer::set_viewport(int width, int height) -> void {
	glViewport(leftX, bottomY, width, height);
}

auto gkit::graphic::opengl::buffer::FrameBuffer::set_viewport() -> void {
	glViewport(leftX, bottomY, fb_width, fb_height);
}

auto gkit::graphic::opengl::buffer::FrameBuffer::bind() const -> void {
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
}

auto gkit::graphic::opengl::buffer::FrameBuffer::unbind() const -> void {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}