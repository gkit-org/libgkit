#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <glad/gl.h>

namespace gkit::graphic::opengl{

	const unsigned int SCR_WIDTH = 500;
	const unsigned int SCR_HEIGHT = 500;

	/// <summary>
	/// 纹理模式
	/// </summary>
	enum class TextureType
	{
		TEXTURE_2D,
		TEXTURE_CUBE_MAP,
		TEXTURE_FRAMEBUFFER
	};

	// ============================================
	// Compare Functions (for depth/stencil tests)
	// ============================================
	enum class CompareFunc {
		Never    = GL_NEVER,
		Less     = GL_LESS,
		Equal    = GL_EQUAL,
		Lequal   = GL_LEQUAL,
		Greater  = GL_GREATER,
		Gequal   = GL_GEQUAL,
		Notequal = GL_NOTEQUAL,
		Always   = GL_ALWAYS
	};

	// ============================================
	// Blend Functions
	// ============================================
	enum class BlendFunc {
		Zero             = GL_ZERO,
		One              = GL_ONE,
		SrcColor         = GL_SRC_COLOR,
		OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
		DstColor         = GL_DST_COLOR,
		OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
		SrcAlpha         = GL_SRC_ALPHA,
		OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
		DstAlpha         = GL_DST_ALPHA,
		OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
		ConstantColor    = GL_CONSTANT_COLOR,
		OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
		ConstantAlpha    = GL_CONSTANT_ALPHA,
		OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA
	};

	// ============================================
	// Blend Equations
	// ============================================
	enum class BlendEquation {
		Add          = GL_FUNC_ADD,
		Subtract     = GL_FUNC_SUBTRACT,
		ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT,
		Min          = GL_MIN,
		Max          = GL_MAX
	};

	// ============================================
	// Cull Face Modes
	// ============================================
	enum class CullFaceMode {
		Front        = GL_FRONT,
		Back         = GL_BACK,
		FrontAndBack = GL_FRONT_AND_BACK
	};

	// ============================================
	// Front Face Winding
	// ============================================
	enum class FrontFace {
		Clockwise    = GL_CW,
		CounterClockwise = GL_CCW
	};

	// ============================================
	// Stencil Operations
	// ============================================
	enum class StencilOp {
		Keep    = GL_KEEP,
		Zero    = GL_ZERO,
		Replace = GL_REPLACE,
		Incr    = GL_INCR,
		IncrWrap = GL_INCR_WRAP,
		Decr    = GL_DECR,
		DecrWrap = GL_DECR_WRAP,
		Invert  = GL_INVERT
	};

	// ============================================
	// Clear Options
	// ============================================
	enum class ClearFlags : GLbitfield {
    Color   = GL_COLOR_BUFFER_BIT,        // Clear Color
    Depth   = GL_DEPTH_BUFFER_BIT,        // Clear Depth
    Stencil = GL_STENCIL_BUFFER_BIT,      // Clear Stencil

    // Common Combinations
    ColorDepth = Color | Depth,           // Color + Depth
    All        = Color | Depth | Stencil   // Clear All
	};

	constexpr ClearFlags operator|(ClearFlags a, ClearFlags b) noexcept {
		return static_cast<ClearFlags>(
			static_cast<GLbitfield>(a) | static_cast<GLbitfield>(b)
		);
	}

}

namespace gkit::graphic::opengl::viewport {

	/// <summary>
	/// Set viewport with custom position and size
	/// </summary>
	auto SetViewport(int x, int y, int width, int height) -> void;

	/// <summary>
	/// Set viewport with custom size (origin at 0, 0)
	/// </summary>
	auto SetViewport(int width, int height) -> void;

}
#endif