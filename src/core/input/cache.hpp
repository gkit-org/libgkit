#pragma once
#include "core/misc/sdl_event_dispatcher.hpp"
#include "gkit/core/input/input.hpp"

#include <atomic>

namespace gkit::input {
    class Cache : public core::templates::Singleton<Cache> {
        friend class core::templates::Singleton<Cache>;
        Cache() { (void)core::SDLEventDispatcher::instance(); }
        Snapshot current;
        std::atomic<std::shared_ptr<const Snapshot>> published{std::make_shared<const Snapshot>()};
        std::vector<core::SDLEventDispatcher::Token> subscriptions;

    public:
        ~Cache();
        auto initialize() -> void;
        auto snapshot() const -> std::shared_ptr<const Snapshot> { return this->published.load(); }
    };
} // namespace gkit::input
