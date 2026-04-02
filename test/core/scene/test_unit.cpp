#include "scene/unit.hpp"
#include <gkit.hpp>
#include <format>
#include <iostream>
#include <memory>
#include <string>

using gkit::scene::Unit;

const auto BEGIN = 5000;
uint32_t times = BEGIN;

// Unit class for debug
class TestUnit : public Unit {
public:
    using Unit::Unit;

    auto _ready() -> void override {
        std::cout << std::format("TestUnit({})::_ready()", this->name) << std::endl;
    }

    auto _process() -> void override {
        std::cout << std::format("TestUnit({})::_process(): num = {}", this->name, this->num) << std::endl;
    }

    // auto _physics_process() -> void override {
    //     std::cout << std::format("TestUnit({})::_physics_process()", this->name) << std::endl;
    // }

    auto _exit() -> void override {
        std::cout << std::format("TestUnit({})::_exit()", this->name) << std::endl;
    }

    int num = 0;
};


class RootUnit : public Unit { 
public:
    using Unit::Unit;

    auto _ready() -> void override {
        std::cout << std::format("RootUnit({})::_ready()", this->name) << std::endl;
        for (auto i = 1; i <= 10; ++i) {
            auto new_child = Unit::create<TestUnit>(std::format("Unit{}", i));
            add_child(std::move(new_child));    
        }
    }
    auto _process() -> void override {
        std::cout << std::format("RootUnit({})::_process()", this->name) << std::endl;
        if (times <= 0) {
            this->remove_child(0);
            times = BEGIN;
        }
        auto result = this->with_child<TestUnit>(0, [](TestUnit& child) -> int {
            child.num += 1;
            return 0;
        });

        times -= 1;
    }
    auto _physics_process() -> void override {}
    auto _exit() -> void override {
        std::cout << std::format("RootUnit({})::_exit()", this->name) << std::endl;
    }
};


int main() {
    auto& app = gkit::Application::instance();
    auto root = Unit::create<RootUnit>("root");
    app.set_root(std::move(root));
    app.run();
}