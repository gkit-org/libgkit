#pragma once

#include "gkit/graphic/render/RenderState.hpp"

#include <cstdint>

/**
 * @brief OpenGL state manager with dirty flag mechanism
 *
 * Owned by opengl::Device. Applies a frontend RenderState snapshot
 * incrementally: only calls GL functions for the parts that changed
 * since the last applied state.
 */
namespace gkit::graphic::opengl {

    class StateManager {
    public:
        StateManager() = default;

        /**
		 * @brief Apply a render state snapshot incrementally
		 *
		 * Compares against the previously applied state and only applies the
		 * changed components (depth/blend/cull/stencil).
		 * @param state Frontend render state snapshot
		 */
        auto apply(const graphic::RenderState& state) -> void;

        /**
		 * @brief Force apply all states (ignore dirty flags)
		 */
        auto force_apply_all() -> void;

    private:
        /// @brief Apply depth state if changed
        auto apply_depth_state() -> void;

        /// @brief Apply blend state if changed
        auto apply_blend_state() -> void;

        /// @brief Apply cull face state if changed
        auto apply_cull_face_state() -> void;

        /// @brief Apply stencil state if changed
        auto apply_stencil_state() -> void;

        /// @brief Currently applied states (shadow state)
        graphic::DepthState depth_state; // Current depth state
        graphic::BlendState blend_state; // Current blend state
        graphic::CullFaceState cull_face_state; // Current cull face state
        graphic::StencilState stencil_state; // Current stencil state
    };

} // namespace gkit::graphic::opengl
