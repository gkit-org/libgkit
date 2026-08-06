#pragma once

#include "gkit/graphic/config.hpp"

#include <cstdint>

namespace gkit::graphic {

    /**
	 * @brief Depth test state
	 */
    struct DepthState {
        bool enabled             = false; // Whether depth test is enabled
        CompareFunc compare_func = CompareFunc::Less; // Depth comparison function
        bool write_mask          = true; // Depth write mask
    };

    /**
	 * @brief Blend state
	 */
    struct BlendState {
        bool enabled           = false; // Whether blending is enabled
        BlendFunc src_rgb      = BlendFunc::One; // Source RGB blend factor
        BlendFunc dst_rgb      = BlendFunc::Zero; // Destination RGB blend factor
        BlendFunc src_alpha    = BlendFunc::One; // Source alpha blend factor
        BlendFunc dst_alpha    = BlendFunc::Zero; // Destination alpha blend factor
        BlendEquation equation = BlendEquation::Add; // Blend equation
    };

    /**
	 * @brief Cull face state
	 */
    struct CullFaceState {
        bool enabled         = false; // Whether cull face is enabled
        CullFaceMode mode    = CullFaceMode::Back; // Cull face mode
        FrontFace front_face = FrontFace::CounterClockwise; // Front face winding order
    };

    /**
	 * @brief Stencil state
	 */
    struct StencilState {
        bool enabled             = false; // Whether stencil test is enabled
        CompareFunc compare_func = CompareFunc::Always; // Stencil comparison function
        uint32_t ref             = 0; // Stencil reference value
        uint32_t read_mask       = 0xFF; // Stencil read mask
        uint32_t write_mask      = 0xFF; // Stencil write mask
        StencilOp fail           = StencilOp::Keep; // Stencil fail operation
        StencilOp z_fail         = StencilOp::Keep; // Stencil depth fail operation
        StencilOp z_pass         = StencilOp::Keep; // Stencil depth pass operation
    };

    /**
	 * @brief Composite render state snapshot (sort key, carried by command)
	 *
	 * Packs the depth/blend/cull/stencil states into one snapshot that is
	 * carried by RenderCommand, used for sorting/deduplication, and applied
	 * incrementally by StateManager.
	 */
    struct RenderState {
        DepthState depth; // Depth test state
        BlendState blend; // Blend state
        CullFaceState cull_face; // Cull face state
        StencilState stencil; // Stencil state

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
