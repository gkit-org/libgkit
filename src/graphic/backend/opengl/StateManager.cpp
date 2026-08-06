#include "graphic/backend/opengl/StateManager.hpp"

#include "graphic/backend/opengl/config.hpp"

namespace gkit::graphic::opengl {

    auto StateManager::apply(const graphic::RenderState& state) -> void {
        // Depth
        if (this->depth_state.enabled != state.depth.enabled ||
            this->depth_state.compare_func != state.depth.compare_func ||
            this->depth_state.write_mask != state.depth.write_mask) {
            this->depth_state = state.depth;
            apply_depth_state();
        }

        // Blend
        if (this->blend_state.enabled != state.blend.enabled || this->blend_state.src_rgb != state.blend.src_rgb ||
            this->blend_state.dst_rgb != state.blend.dst_rgb || this->blend_state.src_alpha != state.blend.src_alpha ||
            this->blend_state.dst_alpha != state.blend.dst_alpha ||
            this->blend_state.equation != state.blend.equation) {
            this->blend_state = state.blend;
            apply_blend_state();
        }

        // Cull face
        if (this->cull_face_state.enabled != state.cull_face.enabled ||
            this->cull_face_state.mode != state.cull_face.mode ||
            this->cull_face_state.front_face != state.cull_face.front_face) {
            this->cull_face_state = state.cull_face;
            apply_cull_face_state();
        }

        // Stencil
        if (this->stencil_state.enabled != state.stencil.enabled ||
            this->stencil_state.compare_func != state.stencil.compare_func ||
            this->stencil_state.ref != state.stencil.ref || this->stencil_state.read_mask != state.stencil.read_mask ||
            this->stencil_state.write_mask != state.stencil.write_mask ||
            this->stencil_state.fail != state.stencil.fail || this->stencil_state.z_fail != state.stencil.z_fail ||
            this->stencil_state.z_pass != state.stencil.z_pass) {
            this->stencil_state = state.stencil;
            apply_stencil_state();
        }
    }

    auto StateManager::force_apply_all() -> void {
        apply_depth_state();
        apply_blend_state();
        apply_cull_face_state();
        apply_stencil_state();
    }

    auto StateManager::apply_depth_state() -> void {
        if (this->depth_state.enabled) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
        glDepthFunc(to_gl_compare_func(this->depth_state.compare_func));
        glDepthMask(this->depth_state.write_mask ? GL_TRUE : GL_FALSE);
    }

    auto StateManager::apply_blend_state() -> void {
        if (this->blend_state.enabled) {
            glEnable(GL_BLEND);
        } else {
            glDisable(GL_BLEND);
        }
        glBlendFuncSeparate(to_gl_blend_func(this->blend_state.src_rgb),
                            to_gl_blend_func(this->blend_state.dst_rgb),
                            to_gl_blend_func(this->blend_state.src_alpha),
                            to_gl_blend_func(this->blend_state.dst_alpha));
        glBlendEquation(to_gl_blend_equation(this->blend_state.equation));
    }

    auto StateManager::apply_cull_face_state() -> void {
        if (this->cull_face_state.enabled) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
        }
        glCullFace(to_gl_cull_face_mode(this->cull_face_state.mode));
        glFrontFace(to_gl_front_face(this->cull_face_state.front_face));
    }

    auto StateManager::apply_stencil_state() -> void {
        if (this->stencil_state.enabled) {
            glEnable(GL_STENCIL_TEST);
        } else {
            glDisable(GL_STENCIL_TEST);
        }
        glStencilFunc(to_gl_compare_func(this->stencil_state.compare_func),
                      this->stencil_state.ref,
                      this->stencil_state.read_mask);
        glStencilOp(to_gl_stencil_op(this->stencil_state.fail),
                    to_gl_stencil_op(this->stencil_state.z_fail),
                    to_gl_stencil_op(this->stencil_state.z_pass));
        glStencilMask(this->stencil_state.write_mask);
    }

} // namespace gkit::graphic::opengl
