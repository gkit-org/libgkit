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

auto gkit::graphic::opengl::buffer::FrameBuffer::AttachColorTexture(const gkit::graphic::opengl::Texture& texture, int slot) -> void {
	Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_2D, texture.GetRenderID(), 0);
}

auto gkit::graphic::opengl::buffer::FrameBuffer::DetachColorTexture(int slot) -> void {
	Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_2D, 0, 0);
}

auto gkit::graphic::opengl::buffer::FrameBuffer::AttachDepthStencil(const RenderBuffer& rbo) -> void {
	Bind();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 
		GL_RENDERBUFFER, rbo.GetRenderID());
}

auto gkit::graphic::opengl::buffer::FrameBuffer::DetachDepthStencil() -> void {
	Bind();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
								GL_RENDERBUFFER, 0);
}

auto gkit::graphic::opengl::buffer::FrameBuffer::Check() -> void {
	Bind();
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "==================================================" << std::endl;
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		std::cout << "==================================================" << std::endl;
	} else {
		std::cout << "FRAMEBUFFER:: Framebuffer is complete!" << std::endl;
	}
	Unbind();
}

auto gkit::graphic::opengl::buffer::FrameBuffer::SetViewport(int x, int y, int width, int height) -> void {
	glViewport(x, y, width, height);
}

auto gkit::graphic::opengl::buffer::FrameBuffer::SetViewport(int width, int height) -> void {
	glViewport(leftX, bottomY, width, height);
}

auto gkit::graphic::opengl::buffer::FrameBuffer::SetViewport() -> void {
	glViewport(leftX, bottomY, fb_width, fb_height);
}

auto gkit::graphic::opengl::buffer::FrameBuffer::Bind() const -> void {
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
}

auto gkit::graphic::opengl::buffer::FrameBuffer::Unbind() const -> void {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}