#pragma once

#include "graphic/window.hpp"
#include "scene/unit.hpp"
#include <atomic>
#include <memory>
#include <vector>

namespace gkit {

    class Application {
    public:
        explicit Application() noexcept;
        virtual ~Application() noexcept;

    public:
        auto run() -> void;
        auto stop() -> void;

        auto set_root(std::unique_ptr<scene::Unit>&& root_ptr) noexcept -> void;

    private:
        std::unique_ptr<scene::Unit> root;
        std::vector<std::unique_ptr<gkit::graphic::Window>> windows;

        std::atomic<bool> running = false;
    }; // class Application

} // namespace gkit