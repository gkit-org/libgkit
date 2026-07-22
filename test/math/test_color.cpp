#include <iostream>

#include <gkit/math/color.hpp>

auto main() -> int {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    std::cout << "=== Color Format Conversion Test ===" << '\n';

    // Test: RGB -> RGBA
    uint32_t rgb = 0xFF8040;
    auto rgba    = rgb_to_rgba(rgb, 128);
    std::cout << "rgb_to_rgba(0xFF8040, 128) = 0x" << std::hex << rgba << std::dec << '\n';

    // Test: get_r/g/b
    std::cout << "get_r(0xFF8040) = " << static_cast<int>(get_r(rgb)) << '\n';
    std::cout << "get_g(0xFF8040) = " << static_cast<int>(get_g(rgb)) << '\n';
    std::cout << "get_b(0xFF8040) = " << static_cast<int>(get_b(rgb)) << '\n';

    std::cout << "\n=== Color Blending Test ===" << '\n';

    // Test: alpha_blend
    uint32_t src = 0x80FF0000; // Red with 50% alpha
    uint32_t dst = 0xFF0000FF; // Blue
    auto blended = alpha_blend(src, dst);
    std::cout << "alpha_blend(0x80FF0000, 0xFF0000FF) = 0x" << std::hex << blended << std::dec << '\n';

    // Test: premultiplied_blend
    auto preblended = premultiplied_blend(src, dst);
    std::cout << "premultiplied_blend(0x80FF0000, 0xFF0000FF) = 0x" << std::hex << preblended << std::dec << '\n';

    std::cout << "\n=== Color Adjustment Test ===" << '\n';

    // Test: brightness
    auto bright = brightness(rgb, 1.5f);
    std::cout << "brightness(0xFF8040, 1.5) = 0x" << std::hex << bright << std::dec << '\n';

    // Test: contrast
    auto contrast_result = contrast(rgb, 1.2f);
    std::cout << "contrast(0xFF8040, 1.2) = 0x" << std::hex << contrast_result << std::dec << '\n';

    // Test: grayscale
    auto gray = grayscale(rgb);
    std::cout << "grayscale(0xFF8040) = 0x" << std::hex << gray << std::dec << '\n';

    // Test: invert
    auto inverted = invert(rgb);
    std::cout << "invert(0xFF8040) = 0x" << std::hex << inverted << std::dec << '\n';

    std::cout << "\n=== HSV <-> RGB Test ===" << '\n';

    // Test: rgb_to_hsv
    auto hsv = rgb_to_hsv(0xFF8040);
    std::cout << "rgb_to_hsv(0xFF8040): H=" << hsv.h << " S=" << hsv.s << " V=" << hsv.v << '\n';

    // Test: hsv_to_rgb
    HSV hsv2{.h = 180.0f, .s = 1.0f, .v = 0.5f};
    auto rgb2 = hsv_to_rgb(hsv2);
    std::cout << "hsv_to_rgb(180, 1.0, 0.5) = 0x" << std::hex << rgb2 << std::dec << '\n';

    std::cout << "\n=== HSL <-> RGB Test ===" << '\n';

    // Test: rgb_to_hsl
    auto hsl = rgb_to_hsl(0xFF8040);
    std::cout << "rgb_to_hsl(0xFF8040): H=" << hsl.h << " S=" << hsl.s << " L=" << hsl.l << '\n';

    // Test: hsl_to_rgb
    HSL hsl2{.h = 60.0f, .s = 1.0f, .l = 0.5f};
    auto rgb3 = hsl_to_rgb(hsl2);
    std::cout << "hsl_to_rgb(60, 1.0, 0.5) = 0x" << std::hex << rgb3 << std::dec << '\n';

    std::cout << "\n=== Utility Test ===" << '\n';

    // Test: lerp_color
    auto lerp_result = lerp_color(0xFF0000, 0x0000FF, 0.5f);
    std::cout << "lerp_color(0xFF0000, 0x0000FF, 0.5) = 0x" << std::hex << lerp_result << std::dec << '\n';

    // Test: is_similar
    bool similar = is_similar(0xFF0000, 0xFE0101, 5);
    std::cout << "is_similar(0xFF0000, 0xFE0101, 5) = " << (similar ? "true" : "false") << '\n';

    // Test: colors constants
    std::cout << "\n=== Color Constants Test ===" << '\n';
    std::cout << "palette_8[0] = 0x" << std::hex << colors::PALETTE_8[0] << std::dec << '\n';
    std::cout << "palette_8[7] = 0x" << std::hex << colors::PALETTE_8[7] << std::dec << '\n';

    std::cout << "\nAll tests completed!" << '\n';
    return 0;
}