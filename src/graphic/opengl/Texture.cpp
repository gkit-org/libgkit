#include "gkit/graphic/opengl/Texture.hpp"

//#include <stb_image.h>
#include <glad/gl.h>

//#include <iostream>

gkit::graphic::opengl::Texture::Texture(const std::string& path, TextureType type)
	:m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), 
	m_Width(0), m_Height(0), m_BPP(0), m_Type(type) {

}

gkit::graphic::opengl::Texture::~Texture() {
    if (m_RendererID != 0) {
        glDeleteTextures(1, &m_RendererID);
        m_RendererID = 0;
    }
}

gkit::graphic::opengl::Texture::Texture(Texture&& other) noexcept
    : m_RendererID(other.m_RendererID)
    , m_FilePath(std::move(other.m_FilePath))
    , m_LocalBuffer(other.m_LocalBuffer)
    , m_Width(other.m_Width)
    , m_Height(other.m_Height)
    , m_BPP(other.m_BPP)
    , m_Type(other.m_Type) {
    other.m_RendererID = 0;
    other.m_LocalBuffer = nullptr;
}

auto gkit::graphic::opengl::Texture::operator=(Texture&& other) noexcept -> Texture& {
    if (this != &other) {
        glDeleteTextures(1, &m_RendererID);
        m_RendererID = other.m_RendererID;
        m_FilePath = std::move(other.m_FilePath);
        m_LocalBuffer = other.m_LocalBuffer;
        m_Width = other.m_Width;
        m_Height = other.m_Height;
        m_BPP = other.m_BPP;
        m_Type = other.m_Type;
        other.m_RendererID = 0;
        other.m_LocalBuffer = nullptr;
    }
    return *this;
}

auto gkit::graphic::opengl::Texture::bind(unsigned int slot) const -> void {
	if (m_Type == TextureType::TEXTURE_CUBE_MAP) {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	} else if (m_Type == TextureType::TEXTURE_2D || m_Type == TextureType::TEXTURE_FRAMEBUFFER) {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}
}

auto gkit::graphic::opengl::Texture::unbind() const -> void {
	if (m_Type == TextureType::TEXTURE_CUBE_MAP) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	} else if (m_Type == TextureType::TEXTURE_2D || m_Type == TextureType::TEXTURE_FRAMEBUFFER) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}