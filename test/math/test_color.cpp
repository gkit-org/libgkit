#include "test_utils.hpp"

#include <array>
#include <iomanip>
#include <iostream>

#include <gkit/math/color.hpp>

using namespace gkit::math; // NOLINT(google-build-using-namespace

auto test_format_conversion() -> bool {
    gkit::test::logln("=== color format conversion ===");

    uint32_t rgb = 0xFF8040;

    auto rgba = rgb_to_rgba(rgb, 128);
    gkit::test::assert_if(rgba == 0x80FF8040, "rgb_to_rgba(0xFF8040, 128) failed");
    gkit::test::logln("  rgb_to_rgba(0xFF8040, 128) = 0x{:X}", rgba);

    gkit::test::assert_if(get_r(rgb) == 0xFF, "get_r(0xFF8040) failed");
    gkit::test::assert_if(get_g(rgb) == 0x80, "get_g(0xFF8040) failed");
    gkit::test::assert_if(get_b(rgb) == 0x40, "get_b(0xFF8040) failed");
    gkit::test::logln("  get_r = {}, get_g = {}, get_b = {}", get_r(rgb), get_g(rgb), get_b(rgb));

    gkit::test::assert_if(rgba_to_rgb(rgba) == rgb, "rgba_to_rgb round-trip failed");
    gkit::test::assert_if(rgba_to_alpha(rgba) == 128, "rgba_to_alpha failed");
    return true;
}


auto test_blending() -> bool {
    gkit::test::logln("=== color blending ===");

    uint32_t src = 0x80FF0000; // Red with 50% alpha
    uint32_t dst = 0xFF0000FF; // Blue

    auto blended = alpha_blend(src, dst);
    gkit::test::logln("  alpha_blend(0x80FF0000, 0xFF0000FF) = 0x{:X}", blended);
    gkit::test::assert_if(get_r(blended) == 128 && get_g(blended) == 0 && get_b(blended) == 126,
                          "alpha_blend of 50% red over blue must yield (128, 0, 126)");

    auto preblended = premultiplied_blend(src, dst);
    gkit::test::logln("  premultiplied_blend(0x80FF0000, 0xFF0000FF) = 0x{:X}", preblended);
    return true;
}


auto test_adjustment() -> bool {
    gkit::test::logln("=== color adjustment ===");

    uint32_t rgb = 0xFF8040;

    auto bright = brightness(rgb, 1.5f);
    gkit::test::assert_if(bright == 0xFFC060, "brightness(0xFF8040, 1.5) failed");
    gkit::test::logln("  brightness(0xFF8040, 1.5) = 0x{:X}", bright);

    auto contrast_result = contrast(rgb, 1.2f);
    gkit::test::logln("  contrast(0xFF8040, 1.2) = 0x{:X}", contrast_result);

    auto gray = grayscale(rgb);
    gkit::test::assert_if(get_r(gray) == get_g(gray) && get_g(gray) == get_b(gray),
                          "grayscale must have equal channels");
    gkit::test::logln("  grayscale(0xFF8040) = 0x{:X}", gray);

    auto inverted = invert(rgb);
    gkit::test::assert_if(inverted == 0xFF007FBF, "invert(0xFF8040) failed");
    gkit::test::logln("  invert(0xFF8040) = 0x{:X}", inverted);
    return true;
}


auto test_hsv_conversion() -> bool {
    gkit::test::logln("=== HSV <-> RGB ===");

    auto hsv = rgb_to_hsv(0xFF8040);
    gkit::test::assert_if(hsv.h >= 0.0f && hsv.h < 360.0f, "HSV hue out of range");
    gkit::test::assert_if(hsv.s >= 0.0f && hsv.s <= 1.0f, "HSV saturation out of range");
    gkit::test::assert_if(hsv.v >= 0.0f && hsv.v <= 1.0f, "HSV value out of range");
    gkit::test::logln("  rgb_to_hsv(0xFF8040): H={} S={} V={}", hsv.h, hsv.s, hsv.v);

    HSV hsv2{.h = 180.0f, .s = 1.0f, .v = 0.5f};
    auto rgb2 = hsv_to_rgb(hsv2);
    gkit::test::assert_if(rgb2 == 0x007F7F, "hsv_to_rgb(180, 1.0, 0.5) must yield half-bright cyan");
    gkit::test::logln("  hsv_to_rgb(180, 1.0, 0.5) = 0x{:X}", rgb2);
    return true;
}


auto test_hsl_conversion() -> bool {
    gkit::test::logln("=== HSL <-> RGB ===");

    auto hsl = rgb_to_hsl(0xFF8040);
    gkit::test::assert_if(hsl.h >= 0.0f && hsl.h < 360.0f, "HSL hue out of range");
    gkit::test::assert_if(hsl.s >= 0.0f && hsl.s <= 1.0f, "HSL saturation out of range");
    gkit::test::assert_if(hsl.l >= 0.0f && hsl.l <= 1.0f, "HSL lightness out of range");
    gkit::test::logln("  rgb_to_hsl(0xFF8040): H={} S={} L={}", hsl.h, hsl.s, hsl.l);

    HSL hsl2{.h = 60.0f, .s = 1.0f, .l = 0.5f};
    auto rgb3 = hsl_to_rgb(hsl2);
    gkit::test::assert_if(rgb3 == 0xFFFF00, "hsl_to_rgb(60, 1.0, 0.5) must yield yellow");
    gkit::test::logln("  hsl_to_rgb(60, 1.0, 0.5) = 0x{:X}", rgb3);
    return true;
}


auto test_color_conversion_values() -> bool {
    constexpr std::array hues{0.0f, 60.0f, 120.0f, 180.0f, 240.0f, 300.0f};
    constexpr std::array<uint32_t, 6> expected{0xFF0000, 0xFFFF00, 0x00FF00, 0x00FFFF, 0x0000FF, 0xFF00FF};
    for (std::size_t i = 0; i < hues.size(); ++i) {
        gkit::test::assert_if(hsv_to_rgb(HSV{.h = hues[i], .s = 1.0f, .v = 1.0f}) == expected[i],
                              "HSV hue sector must produce the expected RGB channels");
        gkit::test::assert_if(hsl_to_rgb(HSL{.h = hues[i], .s = 1.0f, .l = 0.5f}) == expected[i],
                              "HSL hue sector must produce the expected RGB channels");
    }
    constexpr std::array levels{0.0f, 0.5f, 1.0f};
    constexpr std::array<uint32_t, 3> gray{0x000000, 0x7F7F7F, 0xFFFFFF};
    for (std::size_t i = 0; i < levels.size(); ++i) {
        gkit::test::assert_if(hsv_to_rgb(HSV{.h = 180.0f, .s = 0.0f, .v = levels[i]}) == gray[i],
                              "HSV zero saturation must produce grayscale");
        gkit::test::assert_if(hsl_to_rgb(HSL{.h = 180.0f, .s = 0.0f, .l = levels[i]}) == gray[i],
                              "HSL zero saturation must produce grayscale");
    }
    return true;
}


auto test_utility() -> bool {
    gkit::test::logln("=== utility ===");

    auto lerp_result = lerp_color(0xFF0000, 0x0000FF, 0.5f);
    gkit::test::logln("  lerp_color(0xFF0000, 0x0000FF, 0.5) = 0x{:X}", lerp_result);
    gkit::test::assert_if(get_g(lerp_result) == 0, "lerp_color green channel must be 0");

    bool similar = is_similar(0xFF0000, 0xFE0101, 5);
    gkit::test::assert_if(similar, "is_similar(0xFF0000, 0xFE0101, 5) must be true");
    gkit::test::logln("  is_similar(0xFF0000, 0xFE0101, 5) = {}", similar);

    gkit::test::assert_if(!is_similar(0xFF0000, 0x00FF00, 5), "is_similar must be false for distant colors");

    gkit::test::assert_if(colors::PALETTE_8[0] == 0xFF000000, "palette_8[0] must be black");
    gkit::test::assert_if(colors::PALETTE_8[7] == 0xFFFFFFFF, "palette_8[7] must be white");
    gkit::test::logln("  palette_8[0] = 0x{:X}", colors::PALETTE_8[0]);
    gkit::test::logln("  palette_8[7] = 0x{:X}", colors::PALETTE_8[7]);
    return true;
}


auto main() -> int {
    auto test_runner = gkit::test::TestRunner()
                           .add_test_func(test_format_conversion)
                           .add_test_func(test_blending)
                           .add_test_func(test_adjustment)
                           .add_test_func(test_hsv_conversion)
                           .add_test_func(test_hsl_conversion)
                           .add_test_func(test_color_conversion_values)
                           .add_test_func(test_utility);

    test_runner.run();
    return 0;
}
