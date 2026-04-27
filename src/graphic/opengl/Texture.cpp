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

auto gkit::graphic::opengl::Texture::Bind(unsigned int slot) const -> void {
	if (m_Type == TextureType::TEXTURE_CUBE_MAP) {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	} else if (m_Type == TextureType::TEXTURE_2D || m_Type == TextureType::TEXTURE_FRAMEBUFFER) {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}
}

auto gkit::graphic::opengl::Texture::Unbind() const -> void {
	if (m_Type == TextureType::TEXTURE_CUBE_MAP) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	} else if (m_Type == TextureType::TEXTURE_2D || m_Type == TextureType::TEXTURE_FRAMEBUFFER) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}