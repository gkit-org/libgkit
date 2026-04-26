#pragma once

#include <cstdint>
#include <array>


namespace gkit::math {

    // ==========================================
    // Predefined Color Constants
    // ==========================================
    namespace colors {
        /// @brief 8-color basic palette (BGRA format: 0xAABBGGRR)
        inline constexpr std::array<uint32_t, 8> palette_8 = {
            0xFF000000, // 0: Black
            0xFF0000FF, // 1: Red
            0xFF00FF00, // 2: Green
            0xFF00FFFF, // 3: Yellow
            0xFFFF0000, // 4: Blue
            0xFFFF00FF, // 5: Magenta
            0xFFFFFF00, // 6: Cyan
            0xFFFFFFFF  // 7: White
        };

        /// @brief Black (0, 0, 0)
        inline constexpr auto black() { return std::array<float, 3>{ 0.0f, 0.0f, 0.0f }; }

        /// @brief White (1, 1, 1)
        inline constexpr auto white() { return std::array<float, 3>{ 1.0f, 1.0f, 1.0f }; }

        /// @brief Red (1, 0, 0)
        inline constexpr auto red() { return std::array<float, 3>{ 1.0f, 0.0f, 0.0f }; }

        /// @brief Green (0, 1, 0)
        inline constexpr auto green() { return std::array<float, 3>{ 0.0f, 1.0f, 0.0f }; }

        /// @brief Blue (0, 0, 1)
        inline constexpr auto blue() { return std::array<float, 3>{ 0.0f, 0.0f, 1.0f }; }

        /// @brief Yellow (1, 1, 0)
        inline constexpr auto yellow() { return std::array<float, 3>{ 1.0f, 1.0f, 0.0f }; }

        /// @brief Cyan (0, 1, 1)
        inline constexpr auto cyan() { return std::array<float, 3>{ 0.0f, 1.0f, 1.0f }; }

        /// @brief Magenta (1, 0, 1)
        inline constexpr auto magenta() { return std::array<float, 3>{ 1.0f, 0.0f, 1.0f }; }

        /// @brief Gray (0.5, 0.5, 0.5)
        inline constexpr auto gray() { return std::array<float, 3>{ 0.5f, 0.5f, 0.5f }; }

        /// @brief Dark gray (0.25, 0.25, 0.25)
        inline constexpr auto dark_gray() { return std::array<float, 3>{ 0.25f, 0.25f, 0.25f }; }

        /// @brief Light gray (0.75, 0.75, 0.75)
        inline constexpr auto light_gray() { return std::array<float, 3>{ 0.75f, 0.75f, 0.75f }; }

        /// @brief Coral (#FF7F50)
        inline constexpr auto coral() { return std::array<float, 3>{ 1.0f, 0.5f, 0.31f }; }

        /// @brief Tomato (#FF6347)
        inline constexpr auto tomato() { return std::array<float, 3>{ 1.0f, 0.39f, 0.28f }; }

        /// @brief Gold (#FFD700)
        inline constexpr auto gold() { return std::array<float, 3>{ 1.0f, 0.84f, 0.0f }; }

        /// @brief Orange (#FFA500)
        inline constexpr auto orange() { return std::array<float, 3>{ 1.0f, 0.65f, 0.0f }; }

        /// @brief Purple (#800080)
        inline constexpr auto purple() { return std::array<float, 3>{ 0.5f, 0.0f, 0.5f }; }

        /// @brief Violet (#EE82EE)
        inline constexpr auto violet() { return std::array<float, 3>{ 0.93f, 0.51f, 0.93f }; }

        /// @brief Pink (#FFB6C1)
        inline constexpr auto pink() { return std::array<float, 3>{ 1.0f, 0.75f, 0.8f }; }

        /// @brief Brown (#A52A2A)
        inline constexpr auto brown() { return std::array<float, 3>{ 0.65f, 0.16f, 0.16f }; }

        /// @brief Olive (#808000)
        inline constexpr auto olive() { return std::array<float, 3>{ 0.5f, 0.5f, 0.0f }; }

        /// @brief Navy (#000080)
        inline constexpr auto navy() { return std::array<float, 3>{ 0.0f, 0.0f, 0.5f }; }

        /// @brief Teal (#008080)
        inline constexpr auto teal() { return std::array<float, 3>{ 0.0f, 0.5f, 0.5f }; }

        /// @brief Silver (#C0C0C0)
        inline constexpr auto silver() { return std::array<float, 3>{ 0.75f, 0.75f, 0.75f }; }
    } // namespace colors


    // ==========================================
    // Color Format Conversion
    // ==========================================

    /// @brief Convert RGB to RGBA
    /// @param rgb RGB value (0x00BBGGRR)
    /// @param alpha Alpha value (default 255)
    /// @return RGBA value (0xAABBGGRR)
    inline auto rgb_to_rgba(uint32_t rgb, uint8_t alpha = 255) noexcept -> uint32_t {
        return (static_cast<uint32_t>(alpha) << 24) | (rgb & 0x00FFFFFF);
    }

    /// @brief Convert RGBA to RGB (strips alpha)
    /// @param rgba RGBA value
    /// @return RGB value (0x00BBGGRR)
    inline auto rgba_to_rgb(uint32_t rgba) noexcept -> uint32_t {
        return rgba & 0x00FFFFFF;
    }

    /// @brief Extract alpha from RGBA
    /// @param rgba RGBA value
    /// @return Alpha component (0-255)
    inline auto rgba_to_alpha(uint32_t rgba) noexcept -> uint8_t {
        return static_cast<uint8_t>(rgba >> 24);
    }


    // ==========================================
    // RGB Component Extraction
    // ==========================================

    /// @brief Extract red component
    /// @param rgb RGB value
    /// @return Red component (0-255)
    inline auto get_r(uint32_t rgb) noexcept -> uint8_t {
        return static_cast<uint8_t>((rgb >> 16) & 0xFF);
    }

    /// @brief Extract green component
    /// @param rgb RGB value
    /// @return Green component (0-255)
    inline auto get_g(uint32_t rgb) noexcept -> uint8_t {
        return static_cast<uint8_t>((rgb >> 8) & 0xFF);
    }

    /// @brief Extract blue component
    /// @param rgb RGB value
    /// @return Blue component (0-255)
    inline auto get_b(uint32_t rgb) noexcept -> uint8_t {
        return static_cast<uint8_t>(rgb & 0xFF);
    }


    // ==========================================
    // Color Adjustment
    // ==========================================

    /// @brief Adjust color brightness
    /// @param rgb RGB value
    /// @param factor Brightness factor (1.0 = no change)
    /// @return Adjusted RGB value
    inline auto brightness(uint32_t rgb, float factor) noexcept -> uint32_t {
        float r = ((rgb >> 16) & 0xFF) * factor;
        float g = ((rgb >> 8) & 0xFF) * factor;
        float b = (rgb & 0xFF) * factor;
        return (static_cast<uint32_t>(std::min(255.0f, r)) << 16) |
               (static_cast<uint32_t>(std::min(255.0f, g)) << 8) |
               static_cast<uint32_t>(std::min(255.0f, b));
    }

    /// @brief Adjust color contrast
    /// @param rgb RGB value
    /// @param factor Contrast factor (1.0 = no change)
    /// @return Adjusted RGB value
    inline auto contrast(uint32_t rgb, float factor) noexcept -> uint32_t {
        float r = (((rgb >> 16) & 0xFF) - 128.0f) * factor + 128.0f;
        float g = (((rgb >> 8) & 0xFF) - 128.0f) * factor + 128.0f;
        float b = ((rgb & 0xFF) - 128.0f) * factor + 128.0f;
        return (static_cast<uint32_t>(std::max(0.0f, std::min(255.0f, r))) << 16) |
               (static_cast<uint32_t>(std::max(0.0f, std::min(255.0f, g))) << 8) |
               static_cast<uint32_t>(std::max(0.0f, std::min(255.0f, b)));
    }

    /// @brief Convert to grayscale
    /// @param rgb RGB value
    /// @return Grayscale RGB value
    inline auto grayscale(uint32_t rgb) noexcept -> uint32_t {
        auto gray = static_cast<uint8_t>(0.299f * ((rgb >> 16) & 0xFF) +
                                            0.587f * ((rgb >> 8) & 0xFF) +
                                            0.114f * (rgb & 0xFF));
        return (gray << 16) | (gray << 8) | gray;
    }

    /// @brief Invert color
    /// @param rgb RGB value
    /// @return Inverted RGB value
    inline auto invert(uint32_t rgb) noexcept -> uint32_t {
        return 0xFF000000 | (~rgb & 0x00FFFFFF);
    }


    // ==========================================
    // Color Component Setters
    // ==========================================

    /// @brief Set red component
    /// @param rgb RGB value
    /// @param r Red component (0-255)
    /// @return RGB value with red component set
    inline auto set_r(uint32_t rgb, uint8_t r) noexcept -> uint32_t {
        return (rgb & 0x00FFFFFF) | (static_cast<uint32_t>(r) << 16);
    }

    /// @brief Set green component
    /// @param rgb RGB value
    /// @param g Green component (0-255)
    /// @return RGB value with green component set
    inline auto set_g(uint32_t rgb, uint8_t g) noexcept -> uint32_t {
        return (rgb & 0xFFFF00FF) | (static_cast<uint32_t>(g) << 8);
    }

    /// @brief Set blue component
    /// @param rgb RGB value
    /// @param b Blue component (0-255)
    /// @return RGB value with blue component set
    inline auto set_b(uint32_t rgb, uint8_t b) noexcept -> uint32_t {
        return (rgb & 0xFFFFFF00) | static_cast<uint32_t>(b);
    }

    /// @brief Set alpha component
    /// @param rgb RGBA value
    /// @param a Alpha component (0-255)
    /// @return RGBA value with alpha component set
    inline auto set_a(uint32_t rgb, uint8_t a) noexcept -> uint32_t {
        return (static_cast<uint32_t>(a) << 24) | (rgb & 0x00FFFFFF);
    }


    // ==========================================
    // Color Space Structures
    // ==========================================

    /// @brief HSV color structure
    struct HSV {
        float h; /// @brief Hue (0-360)
        float s; /// @brief Saturation (0-1)
        float v; /// @brief Value (0-1)
    };

    /// @brief HSL color structure
    struct HSL {
        float h; /// @brief Hue (0-360)
        float s; /// @brief Saturation (0-1)
        float l; /// @brief Lightness (0-1)
    };


    // ==========================================
    // Color Space Conversion
    // ==========================================

    /// @brief Convert RGB to HSV
    /// @param rgb RGB value
    /// @return HSV structure
    auto rgb_to_hsv(uint32_t rgb) noexcept -> HSV;

    /// @brief Convert HSV to RGB
    /// @param hsv HSV structure
    /// @return RGB value
    auto hsv_to_rgb(const HSV& hsv) noexcept -> uint32_t;

    /// @brief Convert RGB to HSL
    /// @param rgb RGB value
    /// @return HSL structure
    auto rgb_to_hsl(uint32_t rgb) noexcept -> HSL;

    /// @brief Convert HSL to RGB
    /// @param hsl HSL structure
    /// @return RGB value
    auto hsl_to_rgb(const HSL& hsl) noexcept -> uint32_t;


    // ==========================================
    // Color Blending
    // ==========================================

    /// @brief Alpha blend src over dst (standard alpha compositing)
    /// @param src Source color (RGBA)
    /// @param dst Destination color (RGBA)
    /// @return Blended color
    auto alpha_blend(uint32_t src, uint32_t dst) noexcept -> uint32_t;

    /// @brief Premultiplied alpha blend
    /// @param src Source color (RGBA, premultiplied)
    /// @param dst Destination color (RGBA)
    /// @return Blended color
    auto premultiplied_blend(uint32_t src, uint32_t dst) noexcept -> uint32_t;


    // ==========================================
    // Utility Functions
    // ==========================================

    /// @brief Round color to nearest (for antialiasing)
    /// @param color RGB value
    /// @return Rounded RGB value
    inline auto nearest_color(uint32_t color) noexcept -> uint32_t {
        auto r = static_cast<uint8_t>((((color >> 16) & 0xFF) + 127) / 255);
        auto g = static_cast<uint8_t>((((color >> 8) & 0xFF) + 127) / 255);
        auto b = static_cast<uint8_t>(((color & 0xFF) + 127) / 255);
        return (r << 16) | (g << 8) | b;
    }

    /// @brief Linear interpolation between two colors
    /// @param a First color (RGBA)
    /// @param b Second color (RGBA)
    /// @param t Interpolation parameter (0 = a, 1 = b)
    /// @return Interpolated color
    inline auto lerp_color(uint32_t a, uint32_t b, float t) noexcept -> uint32_t {
        float inv_t = 1.0f - t;
        auto r = static_cast<uint8_t>(((a >> 16) & 0xFF) * inv_t + ((b >> 16) & 0xFF) * t);
        auto g = static_cast<uint8_t>(((a >> 8) & 0xFF) * inv_t + ((b >> 8) & 0xFF) * t);
        auto b_r = static_cast<uint8_t>((a & 0xFF) * inv_t + (b & 0xFF) * t);
        auto a_a = static_cast<uint8_t>(((a >> 24) * inv_t + (b >> 24) * t));
        return (static_cast<uint32_t>(a_a) << 24) |
               (static_cast<uint32_t>(r) << 16) |
               (static_cast<uint32_t>(g) << 8) |
               static_cast<uint32_t>(b_r);
    }

    /// @brief Check if two colors are similar
    /// @param a First color
    /// @param b Second color
    /// @param tolerance Similarity tolerance (default 3)
    /// @return True if colors are within tolerance
    inline auto is_similar(uint32_t a, uint32_t b, int tolerance = 3) noexcept -> bool {
        int dr = ((a >> 16) & 0xFF) - ((b >> 16) & 0xFF);
        int dg = ((a >> 8) & 0xFF) - ((b >> 8) & 0xFF);
        int db = (a & 0xFF) - (b & 0xFF);
        return (dr * dr + dg * dg + db * db) <= tolerance * tolerance * 3;
    }

} // namespace gkit::math
