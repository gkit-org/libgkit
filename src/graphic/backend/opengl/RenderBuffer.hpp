#pragma once

#include "gkit/graphic/resource/RenderBuffer.hpp"

#include <cstdint>

/**
 * @brief OpenGL backend renderbuffer
 *
 * Inherits frontend `graphic::RenderBuffer`; used as the depth/stencil
 * attachment of a framebuffer.
 */
namespace gkit::graphic::opengl {

    class RenderBuffer final : public graphic::RenderBuffer {
        friend class Device;

    public:
        RenderBuffer(RenderBuffer&& other) noexcept;
        auto operator=(RenderBuffer&& other) noexcept -> RenderBuffer&;

        ~RenderBuffer() override;

        auto bind() const -> void override;
        auto unbind() const -> void override;

        /**
		 * @brief Get the GL renderbuffer handle (backend escape hatch)
		 */
        [[nodiscard]] auto get_render_id() const -> uint32_t { return this->renderer_id; }

    private:
        explicit RenderBuffer(int width, int height);

        uint32_t renderer_id = 0; // GL renderbuffer handle
    };

} // namespace gkit::graphic::opengl
