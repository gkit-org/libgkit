#pragma once

#include "gkit/graphic/resource/render_buffer.hpp"
#include "graphic/backend/opengl/context.hpp"

#include <cstdint>

/**
 * @brief OpenGL backend renderbuffer
 *
 * Inherits frontend `graphic::RenderBuffer`; used as the depth/stencil
 * attachment of a framebuffer.
 */
namespace gkit::graphic::opengl {

    class RenderBuffer final : public graphic::RenderBuffer, public ContextResource {
        friend class Device;

    public:
        ~RenderBuffer() override;

        auto bind() const -> void override;
        auto unbind() const -> void override;

        /**
         * @brief Get the GL renderbuffer handle (backend escape hatch)
         */
        [[nodiscard]] auto get_render_id() const -> uint32_t { return this->renderer_id; }

        [[nodiscard]] auto width() const noexcept -> int { return buffer_width; }
        [[nodiscard]] auto height() const noexcept -> int { return buffer_height; }

    private:
        explicit RenderBuffer(const std::shared_ptr<const Context>& context, int width, int height);

        int buffer_width;
        int buffer_height;
        uint32_t renderer_id = 0; // GL renderbuffer handle
    };

} // namespace gkit::graphic::opengl
