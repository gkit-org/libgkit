#pragma once

#include "gkit/graphic/resource/frame_buffer.hpp"
#include "gkit/graphic/resource/render_buffer.hpp"
#include "gkit/graphic/resource/texture.hpp"
#include "graphic/backend/opengl/context.hpp"

#include <cstdint>
#include <unordered_map>

/**
 * @brief OpenGL backend framebuffer (FBO)
 *
 * Inherits frontend `graphic::FrameBuffer`; implements the GL off-screen
 * render target.
 */
namespace gkit::graphic::opengl {

    class FrameBuffer final : public graphic::FrameBuffer, public ContextResource {
        friend class Device;

    public:
        ~FrameBuffer() override;

        auto attach_color_texture(std::shared_ptr<const graphic::Texture> texture, int slot) -> void override;
        auto detach_color_texture(int slot) -> void override;
        auto attach_depth_stencil(std::shared_ptr<const graphic::RenderBuffer> rbo) -> void override;
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
        explicit FrameBuffer(const std::shared_ptr<const Context>& context, int width, int height);

        std::unordered_map<int, std::shared_ptr<const graphic::Texture>> color_attachments;
        std::shared_ptr<const graphic::RenderBuffer> depth_stencil;

        uint32_t renderer_id = 0; // GL framebuffer handle
        unsigned int fb_height; // framebuffer height
        unsigned int fb_width; // framebuffer width
        unsigned int left_x   = 0; // viewport left coordinate
        unsigned int bottom_y = 0; // viewport bottom coordinate
    };

} // namespace gkit::graphic::opengl
