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

	class Texture {
	public:
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		/** @brief Move constructor - transfers ownership of GL texture
		 *  @param other Source object to move from (will be invalidated)
		 */
		Texture(Texture&& other) noexcept;

		/** @brief Move assignment - transfers ownership of GL texture
		 *  @param other Source object to move from (will be invalidated)
		 *  @note Releases any existing GL texture before taking ownership
		 */
		auto operator=(Texture&& other) noexcept -> Texture&;

	private:
		uint32_t          m_RendererID;    ///< OpenGL texture ID
		std::string       m_FilePath;      ///< Path to the texture file
		unsigned char*    m_LocalBuffer;   ///< Local buffer for texture data
		int               m_Width;         ///< Texture width in pixels
		int               m_Height;        ///< Texture height in pixels
		int               m_BPP;           ///< Bits per pixel
		TextureType       m_Type;          ///< Texture type (2D, CubeMap, FrameBuffer)

	public:
		/**
		 * @brief Construct a texture (deprecated)
		 * @param path Path to the texture file (can be empty for framebuffer textures)
		 * @param type Texture type (TEXTURE_2D, TEXTURE_CUBE_MAP, TEXTURE_FRAMEBUFFER)
		 * @deprecated Use TextureType enum to specify texture type instead
		 */
		[[deprecated("Use TextureType to specify texture type")]]
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
		[[nodiscard]] inline auto GetWidth() const -> int { return m_Width; }

		/**
		 * @brief Get the texture height
		 * @return Height in pixels
		 */
		[[nodiscard]] inline auto GetHeight() const -> int { return m_Height; }

		/**
		 * @brief Get the Render ID object
		 * @return ID 
		 */
		[[nodiscard]] inline auto GetRenderID() const -> uint32_t { return m_RendererID; }

	private:
		/**
		 * @brief Cube map face file names
		 */
		inline static const std::vector<std::string> faces = {
			"right.jpg",
			"left.jpg",
			"top.jpg",
			"bottom.jpg",
			"front.jpg",
			"back.jpg"
		};
	};

} // namespace gkit::graphic::opengl