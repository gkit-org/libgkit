#pragma once

#include "config.hpp"

#include <cstdint>

namespace gkit::graphic::opengl {

	/**
	* @brief OpenGL state manager with dirty flag mechanism
	*
	* Tracks current OpenGL state and only calls GL functions when state actually changes.
	* Uses singleton pattern for global access.
	*/
	class StateManager
	{
	public:
		// Delete copy/move
		StateManager(const StateManager&) = delete;
		StateManager& operator=(const StateManager&) = delete;
		StateManager(StateManager&&) = delete;
		StateManager& operator=(StateManager&&) = delete;

		/// @brief Get singleton instance
		static auto Get() -> StateManager&;

		// ============================================
		// State Structures
		// ============================================

		struct DepthState {
			bool enabled = false;
			CompareFunc compareFunc = CompareFunc::Less;
			bool writeMask = true;
		};

		struct BlendState {
			bool enabled = false;
			BlendFunc srcRGB = BlendFunc::One;
			BlendFunc dstRGB = BlendFunc::Zero;
			BlendFunc srcAlpha = BlendFunc::One;
			BlendFunc dstAlpha = BlendFunc::Zero;
			BlendEquation equation = BlendEquation::Add;
		};

		struct CullFaceState {
			bool enabled = false;
			CullFaceMode mode = CullFaceMode::Back;
			FrontFace frontFace = FrontFace::CounterClockwise;
		};

		struct StencilState {
			bool enabled = false;
			CompareFunc compareFunc = CompareFunc::Always;
			uint32_t ref = 0;
			uint32_t readMask = 0xFF;
			uint32_t writeMask = 0xFF;
			StencilOp fail = StencilOp::Keep;
			StencilOp zFail = StencilOp::Keep;
			StencilOp zPass = StencilOp::Keep;
		};

		// ============================================
		// State Setters
		// ============================================

		/// @brief Enable or disable depth testing
		auto SetDepthTest(bool enable) -> void;

		/// @brief Set depth test compare function
		auto SetDepthFunc(CompareFunc func) -> void;

		/// @brief Set depth write mask
		auto SetDepthMask(bool write) -> void;

		/// @brief Enable or disable blending
		auto SetBlend(bool enable) -> void;

		/// @brief Set blend factors for RGB and Alpha
		auto SetBlendFunc(BlendFunc srcRGB, BlendFunc dstRGB, BlendFunc srcAlpha, BlendFunc dstAlpha) -> void;

		/// @brief Set blend equation
		auto SetBlendEquation(BlendEquation equation) -> void;

		/// @brief Enable or disable face culling
		auto SetCullFace(bool enable) -> void;

		/// @brief Set cull face mode
		auto SetCullFaceMode(CullFaceMode mode) -> void;

		/// @brief Set front face winding order
		auto SetFrontFace(FrontFace frontFace) -> void;

		/// @brief Enable or disable stencil testing
		auto SetStencilTest(bool enable) -> void;

		/// @brief Set stencil state
		auto SetStencil(CompareFunc func, uint32_t ref, uint32_t mask) -> void;

		/// @brief Set stencil write mask
		auto SetStencilMask(uint32_t mask) -> void;

		/// @brief Set stencil operations
		auto SetStencilOp(StencilOp fail, StencilOp zFail, StencilOp zPass) -> void;

		// ============================================
		// Batch Apply
		// ============================================

		/// @brief Apply all dirty states to OpenGL
		auto Apply() -> void;

		/// @brief Force apply all states (ignore dirty flags)
		auto ForceApplyAll() -> void;

		// ============================================
		// Getters (for querying current state)
		// ============================================

		[[nodiscard]] auto GetDepthState() const -> const DepthState&;
		[[nodiscard]] auto GetBlendState() const -> const BlendState&;
		[[nodiscard]] auto GetCullFaceState() const -> const CullFaceState&;
		[[nodiscard]] auto GetStencilState() const -> const StencilState&;

	private:
		StateManager() = default;

		/// @brief Apply a single state if dirty
		auto ApplyDepthState() -> void;
		auto ApplyBlendState() -> void;
		auto ApplyCullFaceState() -> void;
		auto ApplyStencilState() -> void;

		// Current (shadow) states
		DepthState m_DepthState;
		BlendState m_BlendState;
		CullFaceState m_CullFaceState;
		StencilState m_StencilState;

		// Dirty flags (bitmask)
		uint32_t m_DirtyFlags = 0;

		static constexpr uint32_t DIRTY_DEPTH   = 1 << 0;
		static constexpr uint32_t DIRTY_BLEND   = 1 << 1;
		static constexpr uint32_t DIRTY_CULL    = 1 << 2;
		static constexpr uint32_t DIRTY_STENCIL = 1 << 3;
	};

} // namespace gkit::graphic::opengl
