#pragma once

namespace gkit::graphic {

    /**
     * @brief Sampled image or color attachment in one rendering session.
     * Sprite images are uploaded from immutable resource::Texture data. The device owns
     * the native representation and validates session compatibility when a draw is submitted.
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
