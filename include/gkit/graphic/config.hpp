#pragma once

#include <cstdint>

namespace gkit::graphic {

    /**
	 * @brief Default window size (platform-layer configuration)
	 */
    const unsigned int SCR_WIDTH  = 500;
    const unsigned int SCR_HEIGHT = 500;
    /**
	 * @brief Engine-declared texture slot limit (fixed conservative value)
	 *
	 * Most shaders fit within 8 slots; not chasing hardware limits
	 * (GL_MAX_TEXTURE_IMAGE_UNITS varies by GPU, usually >= 32).
	 * Start up may assert the hardware supports at least this many.
	 */
    static constexpr uint32_t MAX_TEXTURE_SLOTS = 8;

    /**
	 * @brief Texture pattern
	 * @note Backend-agnostic texture type; backends map it to concrete API constants
	 */
    enum class TextureType : std::uint8_t {
        Texture2D,
        TextureCubeMap,
        TextureFramebuffer
    };

    /**
	 * @brief Compare Functions
	 */
    enum class CompareFunc : std::uint8_t {
        Never,
        Less,
        Equal,
        Lequal,
        Greater,
        Gequal,
        Notequal,
        Always
    };

    /**
	 * @brief Blend Functions
	 */
    enum class BlendFunc : std::uint8_t {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        ConstantColor,
        OneMinusConstantColor,
        ConstantAlpha,
        OneMinusConstantAlpha
    };

    /**
	 * @brief Blend Equations
	 */
    enum class BlendEquation : std::uint8_t {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    /**
	 * @brief Cull Face Modes
	 */
    enum class CullFaceMode : std::uint8_t {
        Front,
        Back,
        FrontAndBack
    };

    /**
	 * @brief Front Face Winding
	 */
    enum class FrontFace : std::uint8_t {
        Clockwise,
        CounterClockwise
    };

    /**
	 * @brief Stencil Operations
	 */
    enum class StencilOp : std::uint8_t {
        Keep,
        Zero,
        Replace,
        Incr,
        IncrWrap,
        Decr,
        DecrWrap,
        Invert
    };

    /**
	 * @brief Clear Options
	 */
    enum class ClearFlags : std::uint8_t {
        Color   = 1 << 0, // Clear Color
        Depth   = 1 << 1, // Clear Depth
        Stencil = 1 << 2, // Clear Stencil

        ColorDepth = (1 << 0) | (1 << 1), // Color + Depth
        All        = (1 << 0) | (1 << 1) | (1 << 2) // Clear All
    };

    constexpr auto operator|(ClearFlags a, ClearFlags b) noexcept -> ClearFlags {
        return static_cast<ClearFlags>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
    }

} // namespace gkit::graphic
