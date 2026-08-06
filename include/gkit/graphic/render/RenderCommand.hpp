#pragma once

#include "gkit/graphic/config.hpp"
#include "gkit/graphic/render/RenderState.hpp"
#include "gkit/graphic/resource/FrameBuffer.hpp"

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
        int x      = 0; // Left coordinate
        int y      = 0; // Bottom coordinate
        int width  = 0; // Viewport width
        int height = 0; // Viewport height
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
        const FrameBuffer* target = nullptr; // Render target (nullptr = screen)
        RenderObject* object      = nullptr; // Geometry + material source (lazily uploaded on execute)
        std::optional<Viewport> viewport; // Viewport to set before drawing; empty = target size
        RenderState state; // Render state snapshot (applied before drawing)
        uint32_t instance_count = 1; // 1 = non-instanced
        bool transparent        = false; // Sort front-to-back (opaque) or back-to-front (transparent)
        float depth_key         = 0.0f; // Depth sort key (filled by upper layer)
        bool clear              = false; // Whether to clear the target before drawing
        ClearFlags clear_flags  = ClearFlags::All; // What to clear when clear is set
    };

} // namespace gkit::graphic
