#include "gkit/math/color.hpp"

#include <cmath>

namespace gkit::math {

// ------------------------------
// HSV <-> RGB
// ------------------------------
    auto rgb_to_hsv(uint32_t rgb) noexcept -> HSV {
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

    auto hsv_to_rgb(const HSV& hsv) noexcept -> uint32_t {
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

        uint8_t R = static_cast<uint8_t>((r + m) * 255.0f);
        uint8_t G = static_cast<uint8_t>((g + m) * 255.0f);
        uint8_t B = static_cast<uint8_t>((b + m) * 255.0f);

        return (static_cast<uint32_t>(R) << 16) |
               (static_cast<uint32_t>(G) << 8) |
               static_cast<uint32_t>(B);
    }


// ------------------------------
// HSL <-> RGB
// ------------------------------
    auto rgb_to_hsl(uint32_t rgb) noexcept -> HSL {
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

        return { .h = h, .s = s, .l = l };
    }

    auto hsl_to_rgb(const HSL& hsl) noexcept -> uint32_t {
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

        uint8_t R = static_cast<uint8_t>((r + m) * 255.0f);
        uint8_t G = static_cast<uint8_t>((g + m) * 255.0f);
        uint8_t B = static_cast<uint8_t>((b + m) * 255.0f);

        return (static_cast<uint32_t>(R) << 16) |
               (static_cast<uint32_t>(G) << 8) |
               static_cast<uint32_t>(B);
    }


// ------------------------------
// Alpha Blending
// ------------------------------
    auto alpha_blend(uint32_t src, uint32_t dst) noexcept -> uint32_t {
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

    auto premultiplied_blend(uint32_t src, uint32_t dst) noexcept -> uint32_t {
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

} // namespace gkit::math