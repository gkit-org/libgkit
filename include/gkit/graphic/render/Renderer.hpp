#pragma once

#include "gkit/core/templates/singleton.hpp"
#include "gkit/graphic/config.hpp"
#include "gkit/graphic/render/RenderDevice.hpp"
#include "gkit/graphic/render/RenderObject.hpp"
#include "gkit/graphic/render/RenderQueue.hpp"

#include <memory>

/**
 * @brief Renderer class providing public rendering interface
 *
 * The Renderer provides a unified interface for rendering operations.
 * Uses singleton pattern for global access.
 * Holds a RenderDevice selected via init(Backend).
 */
namespace gkit::graphic {

    class Renderer : public core::templates::Singleton<Renderer> {
        friend class core::templates::Singleton<Renderer>;

    private:
        Renderer() = default;

    public:
        /**
		 * @brief Select the render backend and create the device (defaults to OpenGL)
		 * @param backend backend identifier
		 */
        auto init(Backend backend = Backend::OpenGL) -> void;

        /**
		 * @brief Clear the current framebuffer
		 *
		 * @param flags Bitmask specifying which buffers to clear (e.g., ClearFlags::Color | ClearFlags::Depth)
		 *              Defaults to ClearFlags::All (clears all buffers)
		 */
        auto clear(ClearFlags flags = ClearFlags::All) -> void;

        /**
		 * @brief Enqueue a draw from a reusable render object
		 * @param obj Render object (geometry + material + state)
		 * @param target render target (default nullptr = screen)
		 * @param viewport viewport to use for this draw (default full window)
		 * @note Enqueued into the render queue; executed on flush(). The object is
		 *       non-const because its GPU resources are lazily uploaded on execute.
		 */
        auto draw(RenderObject& obj,
                  const FrameBuffer* target = nullptr,
                  const Viewport& viewport  = Viewport{0, 0, static_cast<int>(SCR_WIDTH), static_cast<int>(SCR_HEIGHT)})
            -> void;

        /**
		 * @brief Execute the queued render commands (sort + apply state + draw)
		 */
        auto flush() -> void;

        /**
		 * @brief Access the current render device
		 */
        [[nodiscard]] auto get_device() -> RenderDevice&;

    private:
        std::unique_ptr<RenderDevice> device;
        RenderQueue queue;
    };

} // namespace gkit::graphic
