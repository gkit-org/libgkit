#include "gkit/graphic/window.hpp"

#include "core/node_internal.hpp"

namespace gkit::graphic {
    auto Window::notify_tree_change(bool entering) -> void {
        if (!tree_changed) return;
        {
            std::lock_guard lock(core::detail::tree_mutex);
            const auto* runtime = core::detail::NodeState::of(*this).runtime;
            // The callback belongs to the original Application and runs only on its coordinator.
            if (!runtime || !runtime->active || runtime->root != application_root ||
                runtime->thread != std::this_thread::get_id())
                return;
        }
        tree_changed(*this, entering);
    }
    auto Window::enter_tree() -> void {
        notify_tree_change(true);
    }
    auto Window::exit_tree() -> void {
        notify_tree_change(false);
    }
} // namespace gkit::graphic
