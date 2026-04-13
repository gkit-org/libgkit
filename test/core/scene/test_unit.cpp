#include <gkit/core/scene/unit.hpp>

#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using gkit::core::scene::Unit;

#define TEST(cond, msg) do { \
    if (!(cond)) { \
        std::cerr << "FAIL: " << msg << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        return false; \
    } else { \
        std::cout << "PASS: " << msg << std::endl; \
    } \
} while(0)

class OtherUnit : public Unit {
public:
    using Unit::Unit;
};

class TestUnit : public Unit {
public:
    using Unit::ready_handler;
    using Unit::process_handler;
    using Unit::physics_process_handler;
    using Unit::exit_handler;

    explicit TestUnit(const std::string& unit_name) : Unit(unit_name), id(unit_name) {}

    static auto create(const std::string& name) -> std::unique_ptr<TestUnit> {
        return Unit::create<TestUnit>(name);
    }

    auto ready() -> void override {
        ++ready_calls;
        timeline.push_back(id + ":ready");
    }

    auto process() -> void override {
        ++process_calls;
        ++value;
        timeline.push_back(id + ":process");
        if (drop_on_process) {
            drop();
        }
    }

    auto physics_process() -> void override {
        ++physics_calls;
        timeline.push_back(id + ":physics");
    }

    auto exit() -> void override {
        ++exit_calls;
        timeline.push_back(id + ":exit");
    }

    std::string id;
    int ready_calls = 0;
    int process_calls = 0;
    int physics_calls = 0;
    int exit_calls = 0;
    int value = 0;
    bool drop_on_process = false;

    static std::vector<std::string> timeline;
};

std::vector<std::string> TestUnit::timeline;

auto test_create_and_with_child() -> bool {
    std::cout << "\n=== test_create_and_with_child ===\n";

    auto parent = TestUnit::create("parent");
    TEST(parent != nullptr, "create<TestUnit> returns non-null");

    auto child = TestUnit::create("child");
    auto* child_ptr = child.get();
    parent->add_child(std::move(child));

    TEST(child_ptr->ready_calls == 1, "add_child triggers child ready once");

    auto value_opt = parent->with_child<TestUnit>(0, [](TestUnit& c) { return c.value; });
    TEST(value_opt == 0, "with_child(index) returns callable result");

    auto value_by_name = parent->with_child<TestUnit>("child", [](TestUnit& c) { return c.value; });
    TEST(value_by_name == 0, "with_child(name) returns callable result");

    bool wrong_type_thrown = false;
    try {
        (void)parent->with_child<OtherUnit>(0, [](OtherUnit&) { return 1; });
    } catch (const std::invalid_argument&) {
        wrong_type_thrown = true;
    }
    TEST(wrong_type_thrown, "with_child throws invalid_argument when type mismatch");

    bool out_of_range_thrown = false;
    try {
        (void)parent->with_child<TestUnit>(99, [](TestUnit&) { return 1; });
    } catch (const std::out_of_range&) {
        out_of_range_thrown = true;
    }
    TEST(out_of_range_thrown, "with_child(index) throws out_of_range when index missing");

    bool name_not_found_thrown = false;
    try {
        (void)parent->with_child<TestUnit>("missing", [](TestUnit&) { return 1; });
    } catch (const std::out_of_range&) {
        name_not_found_thrown = true;
    }
    TEST(name_not_found_thrown, "with_child(name) throws out_of_range when name missing");

    return true;
}

auto test_handlers_and_drop_flow() -> bool {
    std::cout << "\n=== test_handlers_and_drop_flow ===\n";

    auto root = TestUnit::create("root");
    auto keep = TestUnit::create("keep");
    auto drop_later = TestUnit::create("drop_later");

    auto* keep_ptr = keep.get();
    auto* drop_ptr = drop_later.get();

    drop_ptr->drop_on_process = true;

    root->add_child(std::move(keep));
    root->add_child(std::move(drop_later));

    auto _cache_sync = root->with_child<TestUnit>(0, [](TestUnit& c) { return c.process_calls; });

    TestUnit::timeline.clear();
    root->ready_handler();

    TEST(keep_ptr->ready_calls == 2, "ready_handler calls child ready after add_child ready");
    TEST(drop_ptr->ready_calls == 2, "all children participate in ready_handler");
    TEST(root->ready_calls == 1, "ready_handler calls root ready");

    TEST(TestUnit::timeline.size() >= 3, "ready timeline has expected entries");
    TEST(TestUnit::timeline[0] == "keep:ready", "ready order child0 first");
    TEST(TestUnit::timeline[1] == "drop_later:ready", "ready order child1 second");
    TEST(TestUnit::timeline[2] == "root:ready", "ready order root last");

    TestUnit::timeline.clear();
    root->process_handler();

    TEST(keep_ptr->process_calls == 1, "process_handler calls process on normal child");
    TEST(drop_ptr->process_calls == 1, "process_handler calls process on dropping child");
    TEST(root->process_calls == 1, "process_handler calls process on root");
    TEST(drop_ptr->exit_calls == 1, "child dropped during process receives exit");

    auto remain_0 = root->with_child<TestUnit>(0, [](TestUnit& c) { return c.id; });
    TEST(remain_0 == "keep", "remaining child is accessible at index 0");
    bool dropped_missing = false;
    try {
        (void)root->with_child<TestUnit>(1, [](TestUnit& c) { return c.id; });
    } catch (const std::out_of_range&) {
        dropped_missing = true;
    }
    TEST(dropped_missing, "dropped child is no longer accessible");

    auto remain_by_name = root->with_child<TestUnit>("keep", [](TestUnit& c) { return c.id; });
    TEST(remain_by_name == "keep", "remaining child is accessible by name");

    root->physics_process_handler();
    TEST(root->physics_calls == 0, "physics_process_handler is currently no-op");
    TEST(keep_ptr->physics_calls == 0, "physics_process_handler does not cascade currently");

    return true;
}

auto test_remove_child_and_iterators() -> bool {
    std::cout << "\n=== test_remove_child_and_iterators ===\n";

    auto root = TestUnit::create("root");
    std::vector<TestUnit*> children;

    for (int i = 0; i < 3; ++i) {
        auto child = TestUnit::create("child" + std::to_string(i));
        children.push_back(child.get());
        root->add_child(std::move(child));
    }

    root->process_handler();

    int idx = 0;
    for (auto& unit : *root) {
        TEST(&unit == children[static_cast<size_t>(idx)], "forward iterator keeps insertion order");
        ++idx;
    }
    TEST(idx == 3, "forward iterator visits all children");

    idx = 2;
    for (auto it = root->rbegin(); it != root->rend(); ++it) {
        TEST(&(*it) == children[static_cast<size_t>(idx)], "reverse iterator works");
        --idx;
    }
    TEST(idx == -1, "reverse iterator visits all children");

    const auto& const_root = *root;
    idx = 0;
    for (auto& unit : const_root) {
        TEST(&unit == children[static_cast<size_t>(idx)], "const iterator works");
        ++idx;
    }

    auto* removed = children[1];
    root->remove_child(1);
    root->process_handler();

    TEST(removed->exit_calls == 1, "remove_child marks and removes target child");
    bool missing = false;
    try {
        (void)root->with_child<TestUnit>(2, [](TestUnit& c) { return c.id; });
    } catch (const std::out_of_range&) {
        missing = true;
    }
    TEST(missing, "remove_child compacts active view after process frame");

    root->remove_child(1000);
    root->process_handler();
    TEST(true, "remove_child ignores out-of-range index");

    root->remove_child("child0");
    root->process_handler();
    TEST(children[0]->exit_calls == 1, "remove_child(name) marks and removes target child");

    root->remove_child("missing-name");
    root->process_handler();
    TEST(true, "remove_child(name) ignores missing name");

    return true;
}

auto test_exit_handler_order() -> bool {
    std::cout << "\n=== test_exit_handler_order ===\n";

    auto root = TestUnit::create("root");
    auto child0 = TestUnit::create("child0");
    auto child1 = TestUnit::create("child1");

    root->add_child(std::move(child0));
    root->add_child(std::move(child1));

    auto _cache_sync = root->with_child<TestUnit>(0, [](TestUnit& c) { return c.ready_calls; });

    TestUnit::timeline.clear();
    root->exit_handler();

    TEST(TestUnit::timeline.size() >= 3, "exit timeline has expected entries");
    TEST(TestUnit::timeline[0] == "child0:exit", "exit order child0 first");
    TEST(TestUnit::timeline[1] == "child1:exit", "exit order child1 second");
    TEST(TestUnit::timeline[2] == "root:exit", "exit order root last");

    return true;
}

auto main() -> int {
    bool all_passed = true;

    all_passed &= test_create_and_with_child();
    all_passed &= test_handlers_and_drop_flow();
    all_passed &= test_remove_child_and_iterators();
    all_passed &= test_exit_handler_order();

    if (all_passed) {
        std::cout << "\nAll tests passed!" << std::endl;
        return 0;
    }

    std::cerr << "\nSome tests failed." << std::endl;
    return 1;
}
