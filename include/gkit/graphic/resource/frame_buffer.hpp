#pragma once

#include "gkit/graphic/resource/render_buffer.hpp"
#include "gkit/graphic/resource/texture.hpp"

#include <memory>

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
         * @param texture Color attachment from the current session with matching dimensions.
         * @param slot Color attachment slot.
         * @note The framebuffer retains the texture. Finish configuring attachments before submitting draws.
         */
        virtual auto attach_color_texture(std::shared_ptr<const Texture> texture, int slot) -> void = 0;

        /**
         * @brief Detach the color texture from the given slot
         */
        virtual auto detach_color_texture(int slot) -> void = 0;

        /**
         * @brief Attach and retain a depth/stencil renderbuffer from the current session with matching dimensions.
         */
        virtual auto attach_depth_stencil(std::shared_ptr<const RenderBuffer> rbo) -> void = 0;

        /**
         * @brief Detach the depth/stencil attachment
         */
        virtual auto detach_depth_stencil() -> void = 0;

        /**
         * @brief Throw std::runtime_error if the framebuffer is incomplete.
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
