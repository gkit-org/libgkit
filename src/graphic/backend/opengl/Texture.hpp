#pragma once

#include "gkit/graphic/config.hpp"
#include "gkit/graphic/resource/Texture.hpp"

#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief OpenGL backend texture object
 *
 * Inherits frontend `graphic::Texture`.
 * Currently a placeholder implementation (decodes images itself and owns
 * local_buffer); once the resource module is ready, it should hold only a
 * pointer to the resource (see RHI design doc §4.4/§4.5).
 */
namespace gkit::graphic::opengl {

    class Texture final : public graphic::Texture {
    public:
        Texture(const Texture&)                    = delete;
        auto operator=(const Texture&) -> Texture& = delete;

        Texture(Texture&& other) noexcept;
        auto operator=(Texture&& other) noexcept -> Texture&;

        /**
		 * @brief Construct a texture (placeholder)
		 * @param path texture file path (may be empty for framebuffer textures)
		 * @param type texture type
		 * @deprecated textures will be provided by the resource module in the future
		 */
        [[deprecated("In the future, the texture resources provided by the resource management module will be used. "
                     "Currently, they are merely serving as placeholders.")]]
        explicit Texture(const std::string& path, TextureType type = TextureType::Texture2D);

        ~Texture() override;

        auto bind(unsigned int slot) const -> void override;
        auto unbind() const -> void override;

        [[nodiscard]] auto get_width() const -> int override { return this->width; }
        [[nodiscard]] auto get_height() const -> int override { return this->height; }

        /**
		 * @brief Get the GL texture handle (backend escape hatch)
		 */
        [[nodiscard]] inline auto get_renderer_id() const -> uint32_t { return this->renderer_id; }

        /**
		 * @brief Resize the texture storage (framebuffer textures only)
		 *
		 * A framebuffer texture is allocated empty at construction; attaching it
		 * to a FrameBuffer reallocates storage to the FBO size so the texture is
		 * never bound to the global SCR_WIDTH/SCR_HEIGHT constants.
		 * @param width new width in pixels
		 * @param height new height in pixels
		 */
        auto set_size(int width, int height) -> void;

    private:
        inline static const std::vector<std::string> FACES = {
            "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"};

    private:
        uint32_t renderer_id; // GL texture handle
        std::string filepath; // texture file path
        unsigned char* local_buffer; // local pixel data (placeholder; to be owned by the resource module)
        int width; // texture width
        int height; // texture height
        int bpp; // bits per pixel
        TextureType type; // texture type
    };

} // namespace gkit::graphic::opengl
