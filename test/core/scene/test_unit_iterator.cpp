#include "scene/unit.hpp"
#include <gkit.hpp>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cassert>
#include <iterator>

using gkit::scene::Unit;

#define TEST(cond, msg) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " << msg << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        return false; \
    } else { \
        std::cout << "PASS: " << msg << std::endl; \
    } \
} while(0)


class TestUnit : public Unit {
public:
    // using Unit::Unit;
    using Unit::ready_handler;
    using Unit::process_handler;
    using Unit::exit_handler;
    using Unit::parent;

    static auto create(std::string name = "") {
        return Unit::create<TestUnit>(std::move(name));
    }

    int ready_calls = 0;
    int process_calls = 0;
    int physics_calls = 0;
    int exit_calls = 0;
    std::string name_storage;

    // 自定义构造函数
    TestUnit(const std::string& name) : Unit(name), name_storage(name) {}

    void _ready() override {
        ready_calls++;
        std::cout << "TestUnit[" << name_storage << "] _ready()\n";
    }
    void _process() override {
        process_calls++;
        std::cout << "TestUnit[" << name_storage << "] _process()\n";
    }
    void _physics_process() override {
        physics_calls++;
    }
    void _exit() override {
        exit_calls++;
        std::cout << "TestUnit[" << name_storage << "] _exit()\n";
    }
};

// ==================== test ====================

bool test_iterator() {
    std::cout << "\n=== test_iterator ===\n";
    auto parent = TestUnit::create("parent");
    std::vector<TestUnit*> children;
    for (int i = 0; i < 5; ++i) {
        auto child = TestUnit::create("child" + std::to_string(i));
        children.push_back(child.get());
        parent->add_child(std::move(child));
    }
    parent->process_handler();

    int idx = 0;
    for (auto& u : *parent) {
        TEST(&u == children[idx], "iterator order matches addition");
        idx++;
    }
    TEST(idx == 5, "iterated all 5 children");

    idx = 4;
    for (auto it = parent->rbegin(); it != parent->rend(); ++it) {
        TEST(&(*it) == children[idx], "reverse iterator order");
        idx--;
    }
    TEST(idx == -1, "reverse iterated all");

    auto it = parent->begin();
    ++it;
    TEST(&(*it) == children[1], "pre-increment");
    it++;
    TEST(&(*it) == children[2], "post-increment");
    --it;
    TEST(&(*it) == children[1], "pre-decrement");
    it--;
    TEST(&(*it) == children[0], "post-decrement");

    const auto& const_parent = *parent;
    idx = 0;
    for (auto& u : const_parent) {
        TEST(&u == children[idx], "const iterator works");
        idx++;
    }
    return true;
}

// ==================== main function ====================
int main() {
    bool all_passed = true;
    // 仅运行迭代器测试
    all_passed &= test_iterator();

    if (all_passed) {
        std::cout << "\nAll tests passed!\n";
        return 0;
    } else {
        std::cerr << "\nSome tests failed.\n";
        return 1;
    }
}