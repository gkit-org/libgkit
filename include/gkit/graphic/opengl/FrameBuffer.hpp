#pragma once

#include "Texture.hpp"
#include "RenderBuffer.hpp"

/**
 * @brief Frame buffer wrapper for OpenGL framebuffer objects (FBO)
 *
 * A FrameBuffer is an off-screen rendering target that allows rendering
 * to a texture instead of the screen. Commonly used for post-processing
 * effects like color inversion, blur, bloom, etc.
 */
namespace gkit::graphic::opengl::buffer{

	class FrameBuffer {
	public:
		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer& operator=(const FrameBuffer&) = delete;

		/** @brief Move constructor - transfers ownership of GL framebuffer
		 *  @param other Source object to move from (will be invalidated)
		 */
		FrameBuffer(FrameBuffer&& other) noexcept;

		/** @brief Move assignment - transfers ownership of GL framebuffer
		 *  @param other Source object to move from (will be invalidated)
		 *  @note Releases any existing GL framebuffer before taking ownership
		 */
		auto operator=(FrameBuffer&& other) noexcept -> FrameBuffer&;

	private:
		uint32_t       m_RendererID;              ///< OpenGL framebuffer ID
		unsigned int   fb_height;                 ///< Framebuffer height
		unsigned int   fb_width;                  ///< Framebuffer width
		unsigned int   leftX      = 0;            ///< Left coordinate
		unsigned int   bottomY    = 0;            ///< Bottom coordinate
	public:
		/**
		 * @brief Construct a framebuffer
		 */
		FrameBuffer(int width, int height);

		/**
		 * @brief Destructor - deletes the framebuffer
		 */
		~FrameBuffer();

		/**
		 * @brief Attach a color texture to this framebuffer
		 * @param texture The texture to attach as color attachment
		 * @param slot Attachment slot (typically 0 for GL_COLOR_ATTACHMENT0)
		 */
		auto attach_color_texture(const gkit::graphic::opengl::Texture& texture, int slot = 0) -> void;

		/**
		 * @brief Detach a color texture from this framebuffer
		 * @param slot Attachment slot to detach
		 */
		auto detach_color_texture(int slot = 0) -> void;

		/**
		 * @brief Attach a depth-stencil renderbuffer to this framebuffer
		 * @param rbo The renderbuffer to attach as depth-stencil attachment
		 */
		auto attach_depth_stencil(const gkit::graphic::opengl::buffer::RenderBuffer& rbo) -> void;

		/**
		 * @brief Detach the depth-stencil attachment from this framebuffer
		 */
		auto detach_depth_stencil() -> void;

		/**
		 * @brief Check if the framebuffer is complete
		 *
		 * Verifies that the framebuffer is properly configured and ready for rendering.
		 * Prints status message to console.
		 */
		auto check() -> void;

		/**
		 * @brief Set viewport to the framebuffer's default size
		 *
		 * Uses the stored framebuffer dimensions (fb_width, fb_height).
		 */
		auto set_viewport() -> void;

		/**
		 * @brief Set viewport to custom dimensions
		 * @param width Viewport width
		 * @param height Viewport height
		 */
		auto set_viewport(int width, int height) -> void;

		/**
		 * @brief Set custom viewport with offset
		 * @param x Left coordinate
		 * @param y Bottom coordinate
		 * @param width Viewport width
		 * @param height Viewport height
		 */
		auto set_viewport(int x, int y, int width, int height) -> void;

		/**
		 * @brief Bind this framebuffer as the rendering target
		 */
		auto bind() const -> void;

		/**
		 * @brief Unbind this framebuffer, revert to default framebuffer (screen)
		 */
		auto unbind() const -> void;

	};

} // namespace gkit::graphic::opengl::buffer