#pragma once

#include "gkit/graphic/config.hpp"
#include "gkit/graphic/render/render_state.hpp"
#include "gkit/graphic/resource/frame_buffer.hpp"

#include <cstdint>
#include <optional>

namespace gkit::graphic {

    /**
     * @brief Viewport rectangle for a render command
     *
     * Each command carries its own viewport so FBO-targeted commands use the
     * FBO size while screen commands use the window size (GL viewport is global
     * state, so it must be set per command).
     */
    struct Viewport {
        /** @brief Left coordinate. */
        int x = 0;
        /** @brief Bottom coordinate. */
        int y = 0;
        /** @brief Viewport width. */
        int width = 0;
        /** @brief Viewport height. */
        int height = 0;
    };

    class RenderObject;

    /**
     * @brief A single draw command referencing a render object
     *
     * Value type; references (not owns) the RenderObject and its target.
     * The command carries a snapshot of the geometry's render state (so two
     * commands can share one RenderObject with different states) plus per-draw
     * controls (target, viewport, clear, sorting metadata).
     */
    struct RenderCommand {
        /** @brief Render target (nullptr = screen). */
        const FrameBuffer* target = nullptr;
        /** @brief Geometry + material source (lazily uploaded on execute). */
        RenderObject* object = nullptr;
        /** @brief Viewport to set before drawing; empty = target size. */
        std::optional<Viewport> viewport;
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
