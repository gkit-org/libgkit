#pragma once

#include "config.hpp"
#include "gkit/core/scene/singleton.hpp"

#include <cstdint>

namespace gkit::graphic::opengl {

	/**
	 * @brief OpenGL state manager with dirty flag mechanism
	 *
	 * Tracks current OpenGL state and only calls GL functions when state actually changes.
	 * Uses singleton pattern for global access.
	 */
	class StateManager : public core::scene::Singleton<StateManager> {
	public:
		StateManager() = default;

		/** @brief Depth test state structure */
		struct DepthState {
			bool        enabled      = false;                     ///< Whether depth test is enabled
			CompareFunc compareFunc  = CompareFunc::Less;         ///< Depth comparison function
			bool        writeMask    = true;                      ///< Depth write mask
		};

		/** @brief Blend state structure */
		struct BlendState {
			bool          enabled    = false;                     ///< Whether blending is enabled
			BlendFunc     srcRGB     = BlendFunc::One;            ///< Source RGB blend factor
			BlendFunc     dstRGB     = BlendFunc::Zero;           ///< Destination RGB blend factor
			BlendFunc     srcAlpha   = BlendFunc::One;            ///< Source alpha blend factor
			BlendFunc     dstAlpha   = BlendFunc::Zero;           ///< Destination alpha blend factor
			BlendEquation equation   = BlendEquation::Add;       ///< Blend equation
		};

		/** @brief Cull face state structure */
		struct CullFaceState {
			bool          enabled    = false;                      ///< Whether cull face is enabled
			CullFaceMode  mode       = CullFaceMode::Back;         ///< Cull face mode
			FrontFace     frontFace  = FrontFace::CounterClockwise;///< Front face winding order
		};

		/** @brief Stencil state structure */
		struct StencilState {
			bool        enabled      = false;                     ///< Whether stencil test is enabled
			CompareFunc compareFunc  = CompareFunc::Always;       ///< Stencil comparison function
			uint32_t    ref          = 0;                         ///< Stencil reference value
			uint32_t    readMask     = 0xFF;                      ///< Stencil read mask
			uint32_t    writeMask    = 0xFF;                      ///< Stencil write mask
			StencilOp   fail         = StencilOp::Keep;           ///< Stencil fail operation
			StencilOp   zFail        = StencilOp::Keep;           ///< Stencil depth fail operation
			StencilOp   zPass        = StencilOp::Keep;           ///< Stencil depth pass operation
		};

		/** @brief Enable or disable depth testing
		 *  @param enable True to enable depth testing, false to disable
		 */
		auto set_depth_test(bool enable) -> void;

		/** @brief Set depth test compare function
		 *  @param func The comparison function to use
		 */
		auto set_depth_func(CompareFunc func) -> void;

		/** @brief Set depth write mask
		 *  @param write True to enable depth writes, false to disable
		 */
		auto set_depth_mask(bool write) -> void;

		/** @brief Enable or disable blending
		 *  @param enable True to enable blending, false to disable
		 */
		auto set_blend(bool enable) -> void;

		/** @brief Set blend factors for RGB and Alpha
		 *  @param srcRGB Source RGB blend factor
		 *  @param dstRGB Destination RGB blend factor
		 *  @param srcAlpha Source alpha blend factor
		 *  @param dstAlpha Destination alpha blend factor
		 */
		auto set_blend_func(BlendFunc srcRGB, BlendFunc dstRGB, BlendFunc srcAlpha, BlendFunc dstAlpha) -> void;

		/** @brief Set blend equation
		 *  @param equation The blend equation to use
		 */
		auto set_blend_equation(BlendEquation equation) -> void;

		/** @brief Enable or disable face culling
		 *  @param enable True to enable culling, false to disable
		 */
		auto set_cull_face(bool enable) -> void;

		/** @brief Set cull face mode
		 *  @param mode The cull face mode
		 */
		auto set_cull_face_mode(CullFaceMode mode) -> void;

		/** @brief Set front face winding order
		 *  @param frontFace The front face winding order
		 */
		auto set_front_face(FrontFace frontFace) -> void;

		/** @brief Enable or disable stencil testing
		 *  @param enable True to enable stencil testing, false to disable
		 */
		auto set_stencil_test(bool enable) -> void;

		/** @brief Set stencil state
		 *  @param func Stencil comparison function
		 *  @param ref Stencil reference value
		 *  @param mask Stencil read mask
		 */
		auto set_stencil(CompareFunc func, uint32_t ref, uint32_t mask) -> void;

		/** @brief Set stencil write mask
		 *  @param mask Stencil write mask
		 */
		auto set_stencil_mask(uint32_t mask) -> void;

		/** @brief Set stencil operations
		 *  @param fail Operation when stencil test fails
		 *  @param zFail Operation when stencil passes but depth fails
		 *  @param zPass Operation when both stencil and depth pass
		 */
		auto set_stencil_op(StencilOp fail, StencilOp zFail, StencilOp zPass) -> void;

		/** @brief Apply all dirty states to OpenGL */
		auto apply() -> void;

		/** @brief Force apply all states (ignore dirty flags) */
		auto force_apply_all() -> void;

		/** @brief Get current depth state
		 *  @return Reference to the current depth state
		 */
		[[nodiscard]] auto get_depth_state() const -> const DepthState&;

		/** @brief Get current blend state
		 *  @return Reference to the current blend state
		 */
		[[nodiscard]] auto get_blend_state() const -> const BlendState&;

		/** @brief Get current cull face state
		 *  @return Reference to the current cull face state
		 */
		[[nodiscard]] auto get_cull_face_state() const -> const CullFaceState&;

		/** @brief Get current stencil state
		 *  @return Reference to the current stencil state
		 */
		[[nodiscard]] auto get_stencil_state() const -> const StencilState&;

	private:
		/** @brief Apply depth state if dirty */
		auto apply_depth_state() -> void;

		/** @brief Apply blend state if dirty */
		auto apply_blend_state() -> void;

		/** @brief Apply cull face state if dirty */
		auto apply_cull_face_state() -> void;

		/** @brief Apply stencil state if dirty */
		auto apply_stencil_state() -> void;

		/// @brief Current shadow states
		DepthState    m_DepthState;      ///< Current depth state
		BlendState    m_BlendState;      ///< Current blend state
		CullFaceState m_CullFaceState;   ///< Current cull face state
		StencilState  m_StencilState;    ///< Current stencil state

		/// @brief Dirty flags bitmask
		uint32_t m_DirtyFlags = 0;       ///< Bitmask indicating which states need updating

		static constexpr uint32_t DIRTY_DEPTH   = 1 << 0;    ///< Dirty flag for depth state
		static constexpr uint32_t DIRTY_BLEND   = 1 << 1;    ///< Dirty flag for blend state
		static constexpr uint32_t DIRTY_CULL    = 1 << 2;    ///< Dirty flag for cull face state
		static constexpr uint32_t DIRTY_STENCIL = 1 << 3;    ///< Dirty flag for stencil state
	};

} // namespace gkit::graphic::opengl
