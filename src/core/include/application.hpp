#pragma once

#include "scene/unit.hpp"
#include "utils/Singleton.hpp"
#include <atomic>
#include <memory>

namespace gkit {

    class Application : utils::Singleton<Application> {
    public:
        explicit Application() noexcept;
        virtual ~Application() noexcept;

    public:
        auto run() -> void;
        auto stop() -> void;

        auto set_root(std::unique_ptr<scene::Unit>&& root_ptr) noexcept -> void;

    private:
        std::unique_ptr<scene::Unit> root;
        std::atomic<bool> running = false;
    }; // class Application

} // namespace gkit