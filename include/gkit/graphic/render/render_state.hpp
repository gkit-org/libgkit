#pragma once

#include "gkit/graphic/config.hpp"

#include <cstdint>

namespace gkit::graphic {

    /**
     * @brief Depth test state
     */
    struct DepthState {
        /** @brief Whether depth test is enabled. */
        bool enabled = false;
        /** @brief Depth comparison function. */
        CompareFunc compare_func = CompareFunc::Less;
        /** @brief Depth write mask. */
        bool write_mask = true;
    };

    /**
     * @brief Blend state
     */
    struct BlendState {
        /** @brief Whether blending is enabled. */
        bool enabled = false;
        /** @brief Source RGB blend factor. */
        BlendFunc src_rgb = BlendFunc::One;
        /** @brief Destination RGB blend factor. */
        BlendFunc dst_rgb = BlendFunc::Zero;
        /** @brief Source alpha blend factor. */
        BlendFunc src_alpha = BlendFunc::One;
        /** @brief Destination alpha blend factor. */
        BlendFunc dst_alpha = BlendFunc::Zero;
        /** @brief Blend equation. */
        BlendEquation equation = BlendEquation::Add;
    };

    /**
     * @brief Cull face state
     */
    struct CullFaceState {
        /** @brief Whether cull face is enabled. */
        bool enabled = false;
        /** @brief Cull face mode. */
        CullFaceMode mode = CullFaceMode::Back;
        /** @brief Front face winding order. */
        FrontFace front_face = FrontFace::CounterClockwise;
    };

    /**
     * @brief Stencil state
     */
    struct StencilState {
        /** @brief Whether stencil test is enabled. */
        bool enabled = false;
        /** @brief Stencil comparison function. */
        CompareFunc compare_func = CompareFunc::Always;
        /** @brief Stencil reference value. */
        uint32_t ref = 0;
        /** @brief Stencil read mask. */
        uint32_t read_mask = 0xFF;
        /** @brief Stencil write mask. */
        uint32_t write_mask = 0xFF;
        /** @brief Stencil fail operation. */
        StencilOp fail = StencilOp::Keep;
        /** @brief Stencil depth fail operation. */
        StencilOp z_fail = StencilOp::Keep;
        /** @brief Stencil depth pass operation. */
        StencilOp z_pass = StencilOp::Keep;
    };

    /**
     * @brief Composite render state snapshot (sort key, carried by command)
     *
     * Packs the depth/blend/cull/stencil states into one snapshot that is
     * carried by RenderCommand, used for sorting/deduplication, and applied
     * incrementally by StateManager.
     */
    struct RenderState {
        /** @brief Depth test state. */
        DepthState depth;
        /** @brief Blend state. */
        BlendState blend;
        /** @brief Cull face state. */
        CullFaceState cull_face;
        /** @brief Stencil state. */
        StencilState stencil;

        auto operator==(const RenderState& other) const -> bool {
            const auto& a = *this;
            return a.depth.enabled == other.depth.enabled && a.depth.compare_func == other.depth.compare_func &&
                   a.depth.write_mask == other.depth.write_mask && a.blend.enabled == other.blend.enabled &&
                   a.blend.src_rgb == other.blend.src_rgb && a.blend.dst_rgb == other.blend.dst_rgb &&
                   a.blend.src_alpha == other.blend.src_alpha && a.blend.dst_alpha == other.blend.dst_alpha &&
                   a.blend.equation == other.blend.equation && a.cull_face.enabled == other.cull_face.enabled &&
                   a.cull_face.mode == other.cull_face.mode && a.cull_face.front_face == other.cull_face.front_face &&
                   a.stencil.enabled == other.stencil.enabled && a.stencil.compare_func == other.stencil.compare_func &&
                   a.stencil.ref == other.stencil.ref && a.stencil.read_mask == other.stencil.read_mask &&
                   a.stencil.write_mask == other.stencil.write_mask && a.stencil.fail == other.stencil.fail &&
                   a.stencil.z_fail == other.stencil.z_fail && a.stencil.z_pass == other.stencil.z_pass;
        }

        auto operator!=(const RenderState& other) const -> bool { return !(*this == other); }
    };

} // namespace gkit::graphic
