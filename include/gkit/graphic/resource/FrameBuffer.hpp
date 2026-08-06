#pragma once

#include "gkit/graphic/resource/RenderBuffer.hpp"
#include "gkit/graphic/resource/Texture.hpp"

namespace gkit::graphic {

    /**
	 * @brief Framebuffer (frontend abstract interface)
	 *
	 * Off-screen render target combining color texture attachments and
	 * depth/stencil attachments. Not part of the Buffer family (it composes
	 * textures and renderbuffers rather than holding raw data).
	 */
    class FrameBuffer {
    public:
        FrameBuffer()                                      = default;
        FrameBuffer(const FrameBuffer&)                    = delete;
        auto operator=(const FrameBuffer&) -> FrameBuffer& = delete;

        FrameBuffer(FrameBuffer&&) noexcept                    = default;
        auto operator=(FrameBuffer&&) noexcept -> FrameBuffer& = default;

        virtual ~FrameBuffer() = default;

        /**
		 * @brief Attach a color texture to the given slot
		 *
		 * @param texture color attachment (non-const: a framebuffer texture may be
		 *                resized to match this FBO's dimensions on attach)
		 * @param slot color attachment slot
		 */
        virtual auto attach_color_texture(Texture& texture, int slot) -> void = 0;

        /**
		 * @brief Detach the color texture from the given slot
		 */
        virtual auto detach_color_texture(int slot) -> void = 0;

        /**
		 * @brief Attach a depth/stencil renderbuffer
		 */
        virtual auto attach_depth_stencil(const RenderBuffer& rbo) -> void = 0;

        /**
		 * @brief Detach the depth/stencil attachment
		 */
        virtual auto detach_depth_stencil() -> void = 0;

        /**
		 * @brief Check framebuffer completeness
		 */
        virtual auto check() -> void = 0;

        /**
		 * @brief Set the viewport to the framebuffer's default size
		 */
        virtual auto set_viewport() -> void = 0;

        /**
		 * @brief Set a custom viewport size
		 */
        virtual auto set_viewport(int width, int height) -> void = 0;

        /**
		 * @brief Set a custom viewport with offset
		 */
        virtual auto set_viewport(int x, int y, int width, int height) -> void = 0;

        /**
		 * @brief Bind as the current render target
		 */
        virtual auto bind() const -> void = 0;

        /**
		 * @brief Framebuffer width in pixels
		 */
        [[nodiscard]] virtual auto width() const -> int = 0;

        /**
		 * @brief Framebuffer height in pixels
		 */
        [[nodiscard]] virtual auto height() const -> int = 0;

        /**
		 * @brief Unbind, reverting to the default framebuffer (screen)
		 */
        virtual auto unbind() const -> void = 0;
    };

} // namespace gkit::graphic
