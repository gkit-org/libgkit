#pragma once

#include "gkit/core/scene/unit.hpp"
#include <atomic>
#include <memory>
#include <string>
#include <unordered_map>

namespace gkit {
    class Processer {
    public:
        explicit Processer() noexcept;
        virtual ~Processer() noexcept;

    public:
        auto run() -> void;
        auto stop() -> void;

        auto add_service_unit(std::string name, std::unique_ptr<core::scene::Unit>&& unit_ptr) noexcept -> void;
        auto set_root(std::unique_ptr<core::scene::Unit>&& root_ptr) noexcept -> void;

    private:
        std::unique_ptr<core::scene::Unit> root;
        std::unordered_map<std::string, std::unique_ptr<core::scene::Unit>> service_units {};
        std::atomic<bool> running = false;
    }; // class Processer
} // namespace gkit