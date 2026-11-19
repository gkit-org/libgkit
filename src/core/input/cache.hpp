#pragma once
#include "gkit/core/input/input.hpp"

#include <shared_mutex>
#include <unordered_set>

namespace gkit::input {
    class Cache : public core::templates::Singleton<Cache> {
        friend class core::templates::Singleton<Cache>;
        friend class gkit::Input;

        Cache() = default;

        std::shared_mutex mutex;
        std::unordered_set<Key> keys, pressed_keys, released_keys, previous_keys;
        std::unordered_set<MouseButton> buttons, pressed_buttons, released_buttons, previous_buttons;
        std::unordered_set<int> gamepad, pressed_gamepad, released_gamepad;
        MouseMove motion;
        MouseWheel wheel;
        std::uint32_t modifiers          = 0;
        std::uint32_t previous_modifiers = 0;

        /** @brief Clear edges and mouse deltas with the write lock held. */
        auto begin_frame() -> void;

    public:
        auto initialize() -> void;
        auto update(std::span<const platform::Event> events) -> void;
    };
} // namespace gkit::input
