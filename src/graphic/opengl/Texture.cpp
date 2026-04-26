#include "gkit/graphic/opengl/Texture.hpp"

//#include <stb_image.h>
#include <glad/gl.h>

//#include <iostream>

gkit::graphic::opengl::Texture::Texture(const std::string& path, TextureType type)
	:m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), 
	m_Width(0), m_Height(0), m_BPP(0), m_Type(type)
{
	// if (m_Type == TextureType::TEXTURE_2D)
	// {
	// 	stbi_set_flip_vertically_on_load(1);
	// 	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	// 	glGenTextures(1, &m_RendererID));
	// 	glBindTexture(GL_TEXTURE_2D, m_RendererID));

	// 	#pragma region UnKnown
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	// 	#pragma endregion

	// 	if (m_LocalBuffer)
	// 	{
	// 		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
	// 		glGenerateMipmap(GL_TEXTURE_2D);
	// 	}
	// 	if (m_LocalBuffer)
	// 		stbi_image_free(m_LocalBuffer);
	// }
	// else if (m_Type == TextureType::TEXTURE_CUBE_MAP)
	// {
	// 	stbi_set_flip_vertically_on_load(0);

	// 	glGenTextures(1, &m_RendererID);
	// 	glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

	// 	#pragma region UnKnown
	// 	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// 	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// 	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// 	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// 	#pragma endregion

	// 	for (unsigned int i = 0; i < faces.size(); i++)
	// 	{
	// 		std::string fullPath = m_FilePath + faces[i];
	// 		std::cout << fullPath << std::endl;
	// 		m_LocalBuffer = stbi_load(fullPath.c_str(), &m_Width, &m_Height, &m_BPP, 0);
	// 		if (m_LocalBuffer)
	// 		{
	// 			std::cout << "Skybox " << faces[i] << ": " << m_Width << "x" << m_Height << std::endl;
	// 			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_LocalBuffer));
	// 			stbi_image_free(m_LocalBuffer);
	// 		}
	// 		else
	// 		{
	// 			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
	// 			stbi_image_free(m_LocalBuffer);
	// 		}
	// 	}
	// }
	// else if (m_Type == TextureType::TEXTURE_FRAMEBUFFER)
	// {
	// 	glGenTextures(1, &m_RendererID);
	// 	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// }
	// Unbind();
}

gkit::graphic::opengl::Texture::~Texture()
{
    if (m_RendererID != 0) {
        glDeleteTextures(1, &m_RendererID);
        m_RendererID = 0;
    }
}

auto gkit::graphic::opengl::Texture::Bind(unsigned int slot) const -> void
{
	if (m_Type == TextureType::TEXTURE_CUBE_MAP)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}
	else if (m_Type == TextureType::TEXTURE_2D || m_Type == TextureType::TEXTURE_FRAMEBUFFER)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}
}

auto gkit::graphic::opengl::Texture::Unbind() const -> void
{
	if (m_Type == TextureType::TEXTURE_CUBE_MAP)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	else if (m_Type == TextureType::TEXTURE_2D || m_Type == TextureType::TEXTURE_FRAMEBUFFER)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}