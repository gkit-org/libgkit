#pragma once

#include "gkit/graphic/render/RenderCommand.hpp"
#include "gkit/graphic/render/RenderDevice.hpp"

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
		 * @brief Enqueue a command (copied; command is a value type)
		 */
        // cmd is deliberately taken by value: the command is copied into the queue.
        auto submit(RenderCommand cmd) -> void { this->commands.push_back(cmd); }

        /**
		 * @brief Sort and execute all queued commands, then clear
		 * @param device Backend device used to apply state and draw
		 */
        auto flush(RenderDevice& device) -> void;

        /**
		 * @brief Drop all queued commands without executing
		 */
        auto clear() -> void { this->commands.clear(); }

        /// @brief Number of queued commands
        [[nodiscard]] auto size() const -> size_t { return this->commands.size(); }

    private:
        std::vector<RenderCommand> commands;
    };

} // namespace gkit::graphic
