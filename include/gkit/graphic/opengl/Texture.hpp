#pragma once

#include "config.hpp"

#include <vector>
#include <string>

/**
 * @brief Texture wrapper for OpenGL texture objects
 *
 * A Texture represents image data that can be sampled by shaders.
 * Supports 2D textures, cube maps, and framebuffer textures.
 */
namespace gkit::graphic::opengl{

	class Texture
	{
	private:
		uint32_t m_RendererID;          ///< OpenGL texture ID
		std::string m_FilePath;            ///< Path to the texture file
		unsigned char* m_LocalBuffer;      ///< Local buffer for texture data
		int m_Width, m_Height, m_BPP;     ///< Texture dimensions and bits per pixel
		TextureType m_Type;                ///< Texture type (2D, CubeMap, FrameBuffer)

	public:
		/**
		* @brief Construct a texture
		* @param path Path to the texture file (can be empty for framebuffer textures)
		* @param type Texture type (TEXTURE_2D, TEXTURE_CUBE_MAP, TEXTURE_FRAMEBUFFER)
		*/
		Texture(const std::string& path, TextureType type = TextureType::TEXTURE_2D);

		/**
		* @brief Destructor - deletes the texture
		*/
		~Texture();

		/**
		* @brief Bind this texture to a specific slot
		* @param slot Texture slot (0-15)
		*/
		auto Bind(unsigned int slot = 0) const -> void;

		/**
		* @brief Unbind this texture
		*/
		auto Unbind() const -> void;

		/**
		* @brief Get the texture width
		* @return Width in pixels
		*/
		inline auto GetWidth() const -> int { return m_Width; }

		/**
		* @brief Get the texture height
		* @return Height in pixels
		*/
		inline auto GetHeight() const -> int { return m_Height; }

		[[nodiscard]] inline auto GetRenderID() const -> uint32_t { return m_RendererID; }

	private:
		/**
		* @brief Cube map face file names
		*/
		inline static const std::vector<std::string> faces = 
		{
			"right.jpg",
			"left.jpg",
			"top.jpg",
			"bottom.jpg",
			"front.jpg",
			"back.jpg"
		};
	};
	
}