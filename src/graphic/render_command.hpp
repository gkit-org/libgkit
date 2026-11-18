#pragma once

#include "gkit/graphic/config.hpp"
#include "gkit/graphic/render/render_state.hpp"
#include "gkit/graphic/render/viewport_rect.hpp"
#include "gkit/graphic/resource/frame_buffer.hpp"
#include "gkit/graphic/resource/material.hpp"

#include <cstdint>
#include <memory>
#include <optional>

namespace gkit::graphic {

    class RenderObject;

    /**
     * @brief A single draw command referencing a render object
     *
     * Owns the object and target and records the material and render state at submission.
     */
    struct RenderCommand {
        /** @brief Render target (nullptr = screen). */
        std::shared_ptr<const FrameBuffer> target;
        /** @brief Geometry source (lazily uploaded on execute). */
        std::shared_ptr<RenderObject> object;
        /** @brief Shader, textures and uniforms captured at submission. */
        Material material;
        /** @brief ViewportRect to set before drawing; empty = target size. */
        std::optional<ViewportRect> viewport;
        /** @brief Optional scissor rectangle in target pixels. */
        std::optional<ViewportRect> clip;
        /** @brief Render state snapshot (applied before drawing). */
        RenderState state;
        /** @brief 1 = non-instanced. */
        uint32_t instance_count = 1;
        /** @brief Sort front-to-back (opaque) or back-to-front (transparent). */
        bool transparent = false;
        /** @brief Depth sort key (filled by upper layer). */
        float depth_key = 0.0f;
        /** @brief Whether to clear the target before drawing. */
        bool clear = false;
        /** @brief What to clear when clear is set. */
        ClearFlags clear_flags = ClearFlags::All;
    };

} // namespace gkit::graphic
