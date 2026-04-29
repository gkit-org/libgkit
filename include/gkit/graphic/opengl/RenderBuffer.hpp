#pragma once

#include <cstdint>

/**
 * @brief Render buffer wrapper for OpenGL renderbuffer objects
 *
 * A RenderBuffer is an off-screen buffer that stores rendered images but
 * cannot be directly sampled by shaders. Typically used for depth and
 * stencil attachments in a FrameBuffer.
 */
namespace gkit::graphic::opengl::buffer{

	class RenderBuffer {
	public:
		RenderBuffer(const RenderBuffer&) = delete;
		RenderBuffer& operator=(const RenderBuffer&) = delete;

		/** @brief Move constructor - transfers ownership of GL renderbuffer
		 *  @param other Source object to move from (will be invalidated)
		 */
		RenderBuffer(RenderBuffer&& other) noexcept;

		/** @brief Move assignment - transfers ownership of GL renderbuffer
		 *  @param other Source object to move from (will be invalidated)
		 *  @note Releases any existing GL renderbuffer before taking ownership
		 */
		auto operator=(RenderBuffer&& other) noexcept -> RenderBuffer&;

	private:
		uint32_t m_RendererID;  ///< OpenGL renderbuffer ID
	public:
		/**
		 * @brief Construct a renderbuffer
		 */
		RenderBuffer(int width, int height);

		/**
		 * @brief Destructor - deletes the renderbuffer
		 */
		~RenderBuffer();

		/**
		 * @brief Bind this renderbuffer to the current OpenGL context
		 */
		auto Bind() const -> void;

		/**
		 * @brief Unbind this renderbuffer from the current OpenGL context
		 */
		auto Unbind() const -> void;

		[[nodiscard]] auto GetRenderID() const -> uint32_t { return m_RendererID; }
	};

} // namespace gkit::graphic::opengl::buffer