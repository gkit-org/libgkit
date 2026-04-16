#include <gkit/math/color.hpp>
#include <iostream>

using namespace gkit::math;


auto main() -> int {
    std::cout << "=== Color Format Conversion Test ===" << std::endl;

    // Test: RGB -> RGBA
    uint32_t rgb = 0xFF8040;
    auto rgba = rgb_to_rgba(rgb, 128);
    std::cout << "rgb_to_rgba(0xFF8040, 128) = 0x" << std::hex << rgba << std::dec << std::endl;

    // Test: get_r/g/b
    std::cout << "get_r(0xFF8040) = " << static_cast<int>(get_r(rgb)) << std::endl;
    std::cout << "get_g(0xFF8040) = " << static_cast<int>(get_g(rgb)) << std::endl;
    std::cout << "get_b(0xFF8040) = " << static_cast<int>(get_b(rgb)) << std::endl;

    std::cout << "\n=== Color Blending Test ===" << std::endl;

    // Test: alpha_blend
    uint32_t src = 0x80FF0000; // Red with 50% alpha
    uint32_t dst = 0xFF0000FF;   // Blue
    auto blended = alpha_blend(src, dst);
    std::cout << "alpha_blend(0x80FF0000, 0xFF0000FF) = 0x" << std::hex << blended << std::dec << std::endl;

    // Test: premultiplied_blend
    auto preblended = premultiplied_blend(src, dst);
    std::cout << "premultiplied_blend(0x80FF0000, 0xFF0000FF) = 0x" << std::hex << preblended << std::dec << std::endl;

    std::cout << "\n=== Color Adjustment Test ===" << std::endl;

    // Test: brightness
    auto bright = brightness(rgb, 1.5f);
    std::cout << "brightness(0xFF8040, 1.5) = 0x" << std::hex << bright << std::dec << std::endl;

    // Test: contrast
    auto contrast_result = contrast(rgb, 1.2f);
    std::cout << "contrast(0xFF8040, 1.2) = 0x" << std::hex << contrast_result << std::dec << std::endl;

    // Test: grayscale
    auto gray = grayscale(rgb);
    std::cout << "grayscale(0xFF8040) = 0x" << std::hex << gray << std::dec << std::endl;

    // Test: invert
    auto inverted = invert(rgb);
    std::cout << "invert(0xFF8040) = 0x" << std::hex << inverted << std::dec << std::endl;

    std::cout << "\n=== HSV <-> RGB Test ===" << std::endl;

    // Test: rgb_to_hsv
    auto hsv = rgb_to_hsv(0xFF8040);
    std::cout << "rgb_to_hsv(0xFF8040): H=" << hsv.h << " S=" << hsv.s << " V=" << hsv.v << std::endl;

    // Test: hsv_to_rgb
    HSV hsv2{ .h=180.0f, .s=1.0f, .v=0.5f };
    auto rgb2 = hsv_to_rgb(hsv2);
    std::cout << "hsv_to_rgb(180, 1.0, 0.5) = 0x" << std::hex << rgb2 << std::dec << std::endl;

    std::cout << "\n=== HSL <-> RGB Test ===" << std::endl;

    // Test: rgb_to_hsl
    auto hsl = rgb_to_hsl(0xFF8040);
    std::cout << "rgb_to_hsl(0xFF8040): H=" << hsl.h << " S=" << hsl.s << " L=" << hsl.l << std::endl;

    // Test: hsl_to_rgb
    HSL hsl2{ .h=60.0f, .s=1.0f, .l=0.5f };
    auto rgb3 = hsl_to_rgb(hsl2);
    std::cout << "hsl_to_rgb(60, 1.0, 0.5) = 0x" << std::hex << rgb3 << std::dec << std::endl;

    std::cout << "\n=== Utility Test ===" << std::endl;

    // Test: lerp_color
    auto lerp_result = lerp_color(0xFF0000, 0x0000FF, 0.5f);
    std::cout << "lerp_color(0xFF0000, 0x0000FF, 0.5) = 0x" << std::hex << lerp_result << std::dec << std::endl;

    // Test: is_similar
    bool similar = is_similar(0xFF0000, 0xFE0101, 5);
    std::cout << "is_similar(0xFF0000, 0xFE0101, 5) = " << (similar ? "true" : "false") << std::endl;

    // Test: colors constants
    std::cout << "\n=== Color Constants Test ===" << std::endl;
    std::cout << "palette_16[0] = 0x" << std::hex << colors::palette_16[0] << std::dec << std::endl;
    std::cout << "palette_16[7] = 0x" << std::hex << colors::palette_16[7] << std::dec << std::endl;

    std::cout << "\nAll tests completed!" << std::endl;
    return 0;
}