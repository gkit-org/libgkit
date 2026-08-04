#pragma once

#include "gkit/scene/unit.hpp"

#include <atomic>
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

        auto add_service_unit(std::string name,core::UniqueObject&& unit_ptr) noexcept -> void;
        auto set_root(core::UniqueObject&& root_ptr) noexcept -> void;

    private:
        core::UniqueObject root;
        std::unordered_map<std::string, core::UniqueObject> service_units{};
        std::atomic<bool> running = false;
    }; // class Processer
} // namespace gkit