#pragma once

#include "gkit/graphic/resource/texture.hpp"
#include "graphic/backend/opengl/context.hpp"

#include <cstdint>

namespace gkit::graphic::opengl {
    /** @brief OpenGL RGBA color attachment texture. */
    class Texture final : public graphic::Texture, public ContextResource {
        friend class Device;

    public:
        ~Texture() override;

        auto bind(unsigned int slot) const -> void override;
        auto unbind() const -> void override;
        [[nodiscard]] auto get_width() const -> int override { return width; }
        [[nodiscard]] auto get_height() const -> int override { return height; }
        [[nodiscard]] auto get_renderer_id() const -> std::uint32_t { return renderer_id; }

    private:
        Texture(const std::shared_ptr<const Context>& context,
                int width,
                int height,
                const void* pixels = nullptr,
                int pitch          = 0);

        std::uint32_t renderer_id = 0;
        int width;
        int height;
    };
} // namespace gkit::graphic::opengl
