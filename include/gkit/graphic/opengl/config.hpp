#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <glad/gl.h>

namespace gkit::graphic::opengl {

	const unsigned int SCR_WIDTH  = 500;
	const unsigned int SCR_HEIGHT = 500;

	/**
	 * @brief texture pattern
	 */
	enum class TextureType {
		TEXTURE_2D,
		TEXTURE_CUBE_MAP,
		TEXTURE_FRAMEBUFFER
	};

	/**
	 * @brief Compare Functions
	 */
	enum class CompareFunc {
		Never     = GL_NEVER,
		Less      = GL_LESS,
		Equal     = GL_EQUAL,
		Lequal    = GL_LEQUAL,
		Greater   = GL_GREATER,
		Gequal    = GL_GEQUAL,
		Notequal  = GL_NOTEQUAL,
		Always    = GL_ALWAYS
	};

	/**
	 * @brief Blend Functions
	 */
	enum class BlendFunc {
		Zero                  = GL_ZERO,
		One                   = GL_ONE,
		SrcColor              = GL_SRC_COLOR,
		OneMinusSrcColor      = GL_ONE_MINUS_SRC_COLOR,
		DstColor              = GL_DST_COLOR,
		OneMinusDstColor      = GL_ONE_MINUS_DST_COLOR,
		SrcAlpha              = GL_SRC_ALPHA,
		OneMinusSrcAlpha      = GL_ONE_MINUS_SRC_ALPHA,
		DstAlpha              = GL_DST_ALPHA,
		OneMinusDstAlpha      = GL_ONE_MINUS_DST_ALPHA,
		ConstantColor         = GL_CONSTANT_COLOR,
		OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
		ConstantAlpha         = GL_CONSTANT_ALPHA,
		OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA
	};

	/**
	 * @brief Blend Equations
	 */
	enum class BlendEquation {
		Add              = GL_FUNC_ADD,
		Subtract         = GL_FUNC_SUBTRACT,
		ReverseSubtract  = GL_FUNC_REVERSE_SUBTRACT,
		Min              = GL_MIN,
		Max              = GL_MAX
	};

	/**
	 * @brief Cull Face Modes
	 */
	enum class CullFaceMode {
		Front        = GL_FRONT,
		Back         = GL_BACK,
		FrontAndBack = GL_FRONT_AND_BACK
	};

	/**
	 * @brief Front Face Winding
	 */
	enum class FrontFace {
		Clockwise           = GL_CW,
		CounterClockwise    = GL_CCW
	};

	/**
	 * @brief Stencil Operations
	 */
	enum class StencilOp {
		Keep     = GL_KEEP,
		Zero     = GL_ZERO,
		Replace  = GL_REPLACE,
		Incr     = GL_INCR,
		IncrWrap = GL_INCR_WRAP,
		Decr     = GL_DECR,
		DecrWrap = GL_DECR_WRAP,
		Invert   = GL_INVERT
	};

	/**
	 * @brief Clear Options
	 */
	enum class ClearFlags : GLbitfield {
		Color      = GL_COLOR_BUFFER_BIT,        ///< Clear Color
		Depth      = GL_DEPTH_BUFFER_BIT,        ///< Clear Depth
		Stencil    = GL_STENCIL_BUFFER_BIT,      ///< Clear Stencil

		ColorDepth = Color | Depth,              ///< Color + Depth
		All        = Color | Depth | Stencil     ///< Clear All
	};

	constexpr auto operator|(ClearFlags a, ClearFlags b) noexcept -> ClearFlags {
		return static_cast<ClearFlags>(
			static_cast<GLbitfield>(a) | static_cast<GLbitfield>(b)
		);
	}

} // namespace gkit::graphic::opengl

namespace gkit::graphic::opengl::viewport {

	/**
	 * @brief Set viewport with custom position and size
	 *
	 * @param x Left coordinate
	 * @param y Bottom coordinate
	 * @param width Viewport width
	 * @param height Viewport height
	 */
	auto set_viewport(int x, int y, int width, int height) -> void;

	/**
	 * @brief Set viewport with custom size (origin at 0, 0)
	 *
	 * @param width Viewport width
	 * @param height Viewport height
	 */
	auto set_viewport(int width, int height) -> void;

} // namespace gkit::graphic::opengl::viewport
#endif