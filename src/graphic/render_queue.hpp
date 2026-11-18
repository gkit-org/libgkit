#pragma once

#include "graphic/render_command.hpp"
#include "graphic/render_device.hpp"

#include <utility>
#include <vector>

namespace gkit::graphic {

    /**
     * @brief Render queue: collects commands, sorts, and executes at flush time
     *
     * Collects RenderCommand values during a frame and executes them in
     * `flush()` after sorting. State application and drawing go through the
     * frontend RenderDevice abstraction; the queue never touches GL directly.
     */
    class RenderQueue {
    public:
        /**
         * @brief Enqueue a command and the resources it owns.
         */
        auto submit(RenderCommand cmd) -> void { this->commands.push_back(std::move(cmd)); }

        /**
         * @brief Sort and execute all queued commands, then clear
         * @param device Backend device used to apply state and draw
         * @param session Identifies the context used for geometry uploads.
         * @param screen Default viewport for the window.
         */
        auto flush(RenderDevice& device, const std::shared_ptr<const void>& session, Viewport screen) -> void;

        /**
         * @brief Drop all queued commands without executing
         */
        auto clear() -> void { this->commands.clear(); }

        /**
         * @brief Number of queued commands
         */
        [[nodiscard]] auto size() const -> size_t { return this->commands.size(); }

    private:
        std::vector<RenderCommand> commands;
    };

} // namespace gkit::graphic
