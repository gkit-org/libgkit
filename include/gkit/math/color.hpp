#pragma once

#include <cstdint>
#include <cmath>
#include <array>


namespace gkit::math {

// ------------------------------
// Predefined Color Constants
// ------------------------------
    namespace colors {
        // Basic 8-color palette (BGRA format: 0xAABBGGRR)
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

        // Basic colors
        inline constexpr auto black() { return std::array<float, 3>{ 0.0f, 0.0f, 0.0f }; }
        inline constexpr auto white() { return std::array<float, 3>{ 1.0f, 1.0f, 1.0f }; }
        inline constexpr auto red() { return std::array<float, 3>{ 1.0f, 0.0f, 0.0f }; }
        inline constexpr auto green() { return std::array<float, 3>{ 0.0f, 1.0f, 0.0f }; }
        inline constexpr auto blue() { return std::array<float, 3>{ 0.0f, 0.0f, 1.0f }; }
        inline constexpr auto yellow() { return std::array<float, 3>{ 1.0f, 1.0f, 0.0f }; }
        inline constexpr auto cyan() { return std::array<float, 3>{ 0.0f, 1.0f, 1.0f }; }
        inline constexpr auto magenta() { return std::array<float, 3>{ 1.0f, 0.0f, 1.0f }; }

        // Grayscale
        inline constexpr auto gray() { return std::array<float, 3>{ 0.5f, 0.5f, 0.5f }; }
        inline constexpr auto dark_gray() { return std::array<float, 3>{ 0.25f, 0.25f, 0.25f }; }
        inline constexpr auto light_gray() { return std::array<float, 3>{ 0.75f, 0.75f, 0.75f }; }

        // HTML color names
        namespace html {
            inline constexpr auto coral() { return std::array<float, 3>{ 1.0f, 0.5f, 0.31f }; }
            inline constexpr auto tomato() { return std::array<float, 3>{ 1.0f, 0.39f, 0.28f }; }
            inline constexpr auto gold() { return std::array<float, 3>{ 1.0f, 0.84f, 0.0f }; }
            inline constexpr auto orange() { return std::array<float, 3>{ 1.0f, 0.65f, 0.0f }; }
            inline constexpr auto purple() { return std::array<float, 3>{ 0.5f, 0.0f, 0.5f }; }
            inline constexpr auto violet() { return std::array<float, 3>{ 0.93f, 0.51f, 0.93f }; }
            inline constexpr auto pink() { return std::array<float, 3>{ 1.0f, 0.75f, 0.8f }; }
            inline constexpr auto brown() { return std::array<float, 3>{ 0.65f, 0.16f, 0.16f }; }
            inline constexpr auto olive() { return std::array<float, 3>{ 0.5f, 0.5f, 0.0f }; }
            inline constexpr auto navy() { return std::array<float, 3>{ 0.0f, 0.0f, 0.5f }; }
            inline constexpr auto teal() { return std::array<float, 3>{ 0.0f, 0.5f, 0.5f }; }
            inline constexpr auto silver() { return std::array<float, 3>{ 0.75f, 0.75f, 0.75f }; }
        } // namespace html
    } // namespace colors


// ------------------------------
// Color Operation Functions
// ------------------------------
    // Format conversion
    inline auto rgb_to_rgba(uint32_t rgb, uint8_t alpha = 255) noexcept -> uint32_t {
        return (static_cast<uint32_t>(alpha) << 24) | (rgb & 0x00FFFFFF);
    }

    inline auto rgba_to_rgb(uint32_t rgba) noexcept -> uint32_t {
        return rgba & 0x00FFFFFF;
    }

    inline auto rgba_to_alpha(uint32_t rgba) noexcept -> uint8_t {
        return static_cast<uint8_t>(rgba >> 24);
    }

    // Extract RGB components
    inline auto get_r(uint32_t rgb) noexcept -> uint8_t {
        return static_cast<uint8_t>((rgb >> 16) & 0xFF);
    }
    inline auto get_g(uint32_t rgb) noexcept -> uint8_t {
        return static_cast<uint8_t>((rgb >> 8) & 0xFF);
    }
    inline auto get_b(uint32_t rgb) noexcept -> uint8_t {
        return static_cast<uint8_t>(rgb & 0xFF);
    }

    // Color blending
    inline auto alpha_blend(uint32_t src, uint32_t dst) noexcept -> uint32_t {
        auto sa = static_cast<uint8_t>(src >> 24);
        auto da = static_cast<uint8_t>(dst >> 24);

        // Fully transparent source: return destination
        if (sa == 0) return dst;
        // Fully opaque source: return source
        if (sa == 255) return src;

        // Alpha blending: result = src * alpha + dst * (1 - alpha)
        float alpha = sa / 255.0f;
        float inv_alpha = 1.0f - alpha;

        auto r = static_cast<uint8_t>(((src >> 16) & 0xFF) * alpha + ((dst >> 16) & 0xFF) * inv_alpha);
        auto g = static_cast<uint8_t>(((src >> 8) & 0xFF) * alpha + ((dst >> 8) & 0xFF) * inv_alpha);
        auto b = static_cast<uint8_t>((src & 0xFF) * alpha + (dst & 0xFF) * inv_alpha);
        auto a = static_cast<uint8_t>(sa + da * inv_alpha);

        return (static_cast<uint32_t>(a) << 24) |
               (static_cast<uint32_t>(r) << 16) |
               (static_cast<uint32_t>(g) << 8) |
               static_cast<uint32_t>(b);
    }

    // Premultiplied alpha blending
    inline auto premultiplied_blend(uint32_t src, uint32_t dst) noexcept -> uint32_t {
        auto sa = static_cast<uint8_t>(src >> 24);
        auto da = static_cast<uint8_t>(dst >> 24);

        // Fully transparent source: return destination
        if (sa == 0) return dst;
        // Fully opaque source: return source
        if (sa == 255) return src;

        // Premultiplied alpha: src already multiplied by its alpha
        float alpha = sa / 255.0f;
        auto ra = static_cast<uint8_t>(std::min(255, sa + da));

        auto r = static_cast<uint8_t>(((src >> 16) & 0xFF) + ((dst >> 16) & 0xFF) * (1.0f - alpha));
        auto g = static_cast<uint8_t>(((src >> 8) & 0xFF) + ((dst >> 8) & 0xFF) * (1.0f - alpha));
        auto b = static_cast<uint8_t>((src & 0xFF) + (dst & 0xFF) * (1.0f - alpha));

        return (static_cast<uint32_t>(ra) << 24) |
               (static_cast<uint32_t>(r) << 16) |
               (static_cast<uint32_t>(g) << 8) |
               static_cast<uint32_t>(b);
    }

    // Brightness adjustment
    inline auto brightness(uint32_t rgb, float factor) noexcept -> uint32_t {
        float r = ((rgb >> 16) & 0xFF) * factor;
        float g = ((rgb >> 8) & 0xFF) * factor;
        float b = (rgb & 0xFF) * factor;
        return (static_cast<uint32_t>(std::min(255.0f, r)) << 16) |
               (static_cast<uint32_t>(std::min(255.0f, g)) << 8) |
               static_cast<uint32_t>(std::min(255.0f, b));
    }

    // Contrast adjustment
    inline auto contrast(uint32_t rgb, float factor) noexcept -> uint32_t {
        float r = (((rgb >> 16) & 0xFF) - 128.0f) * factor + 128.0f;
        float g = (((rgb >> 8) & 0xFF) - 128.0f) * factor + 128.0f;
        float b = ((rgb & 0xFF) - 128.0f) * factor + 128.0f;
        return (static_cast<uint32_t>(std::max(0.0f, std::min(255.0f, r))) << 16) |
               (static_cast<uint32_t>(std::max(0.0f, std::min(255.0f, g))) << 8) |
               static_cast<uint32_t>(std::max(0.0f, std::min(255.0f, b)));
    }

    // Grayscale conversion
    inline auto grayscale(uint32_t rgb) noexcept -> uint32_t {
        uint8_t gray = static_cast<uint8_t>(0.299f * ((rgb >> 16) & 0xFF) +
                                            0.587f * ((rgb >> 8) & 0xFF) +
                                            0.114f * (rgb & 0xFF));
        return (gray << 16) | (gray << 8) | gray;
    }

    // Color inversion
    inline auto invert(uint32_t rgb) noexcept -> uint32_t {
        return 0xFF000000 | (~rgb & 0x00FFFFFF);
    }

    // Set color components
    inline auto set_r(uint32_t rgb, uint8_t r) noexcept -> uint32_t {
        return (rgb & 0x00FFFFFF) | (static_cast<uint32_t>(r) << 16);
    }
    inline auto set_g(uint32_t rgb, uint8_t g) noexcept -> uint32_t {
        return (rgb & 0xFFFF00FF) | (static_cast<uint32_t>(g) << 8);
    }
    inline auto set_b(uint32_t rgb, uint8_t b) noexcept -> uint32_t {
        return (rgb & 0xFFFFFF00) | static_cast<uint32_t>(b);
    }
    inline auto set_a(uint32_t rgb, uint8_t a) noexcept -> uint32_t {
        return (static_cast<uint32_t>(a) << 24) | (rgb & 0x00FFFFFF);
    }


    // ------------------------------
    // HSV <-> RGB Conversion
    // ------------------------------
    struct HSV {
        float h; // 0-360 Hue
        float s; // 0-1 Saturation
        float v; // 0-1 Value
    };

    inline auto rgb_to_hsv(uint32_t rgb) noexcept -> HSV {
        float r = ((rgb >> 16) & 0xFF) / 255.0f;
        float g = ((rgb >> 8) & 0xFF) / 255.0f;
        float b = (rgb & 0xFF) / 255.0f;

        float max_val = std::max(std::max(r, g), b);
        float min_val = std::min(std::min(r, g), b);
        float delta = max_val - min_val;

        // Value
        float v = max_val;

        // Saturation
        float s = (max_val > 0.0f) ? delta / max_val : 0.0f;

        // Hue
        float h = 0.0f;
        if (delta > 0.0f) {
            if (max_val == r) h = 60.0f * std::fmod((g - b) / delta, 6.0f);
            else if (max_val == g) h = 60.0f * ((b - r) / delta + 2.0f);
            else h = 60.0f * ((r - g) / delta + 4.0f);
        }
        if (h < 0.0f) h += 360.0f;

        return { .h = h, .s = s, .v = v };
    }

    inline auto hsv_to_rgb(const HSV& hsv) noexcept -> uint32_t {
        float h = hsv.h;
        float s = hsv.s;
        float v = hsv.v;

        float c = v * s;
        float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
        float m = v - c;

        float r, g, b;
        if (h < 60.0f) { r = c; g = x; b = 0; }
        else if (h < 120.0f) { r = x; g = c; b = 0; }
        else if (h < 180.0f) { r = 0; g = c; b = x; }
        else if (h < 240.0f) { r = 0; g = x; b = c; }
        else if (h < 300.0f) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }

        auto R = static_cast<uint8_t>((r + m) * 255.0f);
        auto G = static_cast<uint8_t>((g + m) * 255.0f);
        auto B = static_cast<uint8_t>((b + m) * 255.0f);

        return (static_cast<uint32_t>(R) << 16) |
               (static_cast<uint32_t>(G) << 8) |
               static_cast<uint32_t>(B);
    }


    // ------------------------------
    // RGB <-> HSL Conversion
    // ------------------------------
    struct HSL {
        float h; // 0-360 Hue
        float s; // 0-1 Saturation
        float l; // 0-1 Lightness
    };

    inline auto rgb_to_hsl(uint32_t rgb) noexcept -> HSL {
        float r = ((rgb >> 16) & 0xFF) / 255.0f;
        float g = ((rgb >> 8) & 0xFF) / 255.0f;
        float b = (rgb & 0xFF) / 255.0f;

        float max_val = std::max(std::max(r, g), b);
        float min_val = std::min(std::min(r, g), b);
        float delta = max_val - min_val;

        float l = (max_val + min_val) / 2.0f;

        float s = 0.0f;
        if (delta > 0.0f) {
            s = l > 0.5f ? delta / (2.0f - max_val - min_val) : delta / (max_val + min_val);
        }

        float h = 0.0f;
        if (delta > 0.0f) {
            if (max_val == r) h = 60.0f * std::fmod((g - b) / delta, 6.0f);
            else if (max_val == g) h = 60.0f * ((b - r) / delta + 2.0f);
            else h = 60.0f * ((r - g) / delta + 4.0f);
        }
        if (h < 0.0f) h += 360.0f;

        return { .h=h, .s=s, .l=l };
    }

    inline auto hsl_to_rgb(const HSL& hsl) noexcept -> uint32_t {
        float h = hsl.h;
        float s = hsl.s;
        float l = hsl.l;

        float c = (1.0f - std::abs(2.0f * l - 1.0f)) * s;
        float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
        float m = l - c / 2.0f;

        float r, g, b;
        if (h < 60.0f) { r = c; g = x; b = 0; }
        else if (h < 120.0f) { r = x; g = c; b = 0; }
        else if (h < 180.0f) { r = 0; g = c; b = x; }
        else if (h < 240.0f) { r = 0; g = x; b = c; }
        else if (h < 300.0f) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }

        auto R = static_cast<uint8_t>((r + m) * 255.0f);
        auto G = static_cast<uint8_t>((g + m) * 255.0f);
        auto B = static_cast<uint8_t>((b + m) * 255.0f);

        return (static_cast<uint32_t>(R) << 16) |
               (static_cast<uint32_t>(G) << 8) |
               static_cast<uint32_t>(B);
    }


    // ------------------------------
    // Utility Functions
    // ------------------------------
    // Color rounding (for antialiasing)
    inline auto nearest_color(uint32_t color) noexcept -> uint32_t {
        auto r = static_cast<uint8_t>((((color >> 16) & 0xFF) + 127) / 255);
        auto g = static_cast<uint8_t>((((color >> 8) & 0xFF) + 127) / 255);
        auto b = static_cast<uint8_t>(((color & 0xFF) + 127) / 255);
        return (r << 16) | (g << 8) | b;
    }

    // Linear interpolation
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

    // Check if colors are similar
    inline auto is_similar(uint32_t a, uint32_t b, int tolerance = 3) noexcept -> bool {
        int dr = ((a >> 16) & 0xFF) - ((b >> 16) & 0xFF);
        int dg = ((a >> 8) & 0xFF) - ((b >> 8) & 0xFF);
        int db = (a & 0xFF) - (b & 0xFF);
        return (dr * dr + dg * dg + db * db) <= tolerance * tolerance * 3;
    }

} // namespace gkit::math