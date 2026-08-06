#pragma once

#include "gkit/graphic/resource/FrameBuffer.hpp"
#include "gkit/graphic/resource/RenderBuffer.hpp"
#include "gkit/graphic/resource/Texture.hpp"

#include <cstdint>

/**
 * @brief OpenGL backend framebuffer (FBO)
 *
 * Inherits frontend `graphic::FrameBuffer`; implements the GL off-screen
 * render target.
 */
namespace gkit::graphic::opengl {

    class FrameBuffer final : public graphic::FrameBuffer {
        friend class Device;

    public:
        FrameBuffer(FrameBuffer&& other) noexcept;
        auto operator=(FrameBuffer&& other) noexcept -> FrameBuffer&;

        ~FrameBuffer() override;

        auto attach_color_texture(graphic::Texture& texture, int slot) -> void override;
        auto detach_color_texture(int slot) -> void override;
        auto attach_depth_stencil(const graphic::RenderBuffer& rbo) -> void override;
        auto detach_depth_stencil() -> void override;
        auto check() -> void override;
        auto set_viewport() -> void override;
        auto set_viewport(int width, int height) -> void override;
        auto set_viewport(int x, int y, int width, int height) -> void override;
        auto bind() const -> void override;
        auto unbind() const -> void override;

        [[nodiscard]] auto width() const -> int override;
        [[nodiscard]] auto height() const -> int override;

    private:
        explicit FrameBuffer(int width, int height);

        uint32_t renderer_id = 0; // GL framebuffer handle
        unsigned int fb_height; // framebuffer height
        unsigned int fb_width; // framebuffer width
        unsigned int left_x   = 0; // viewport left coordinate
        unsigned int bottom_y = 0; // viewport bottom coordinate
    };

} // namespace gkit::graphic::opengl
