#pragma once

namespace gkit::graphic {

    /**
	 * @brief Texture (frontend abstract interface)
	 *
	 * A thin binding between the CPU resource (pixel data, owned by the resource
	 * module) and the GPU texture object (owned by the backend).
	 * Does not own pixel data or decode images.
	 *
	 * TODO(future): once the resource module is ready, the constructor/factory
	 *               should take a resource handle (shared_ptr<resource::Texture>
	 *               or GUID); see RHI design doc §4.4/§4.5.
	 *               `opengl::Texture` is currently a placeholder implementation.
	 */
    class Texture {
    public:
        Texture()                                  = default;
        Texture(const Texture&)                    = delete;
        auto operator=(const Texture&) -> Texture& = delete;

        Texture(Texture&&) noexcept                    = default;
        auto operator=(Texture&&) noexcept -> Texture& = default;

        virtual ~Texture() = default;

        /**
		 * @brief Bind to the given slot
		 */
        virtual auto bind(unsigned int slot) const -> void = 0;

        /**
		 * @brief Unbind
		 */
        virtual auto unbind() const -> void = 0;

        /**
		 * @brief Texture width in pixels
		 */
        [[nodiscard]] virtual auto get_width() const -> int = 0;

        /**
		 * @brief Texture height in pixels
		 */
        [[nodiscard]] virtual auto get_height() const -> int = 0;
    };

} // namespace gkit::graphic
