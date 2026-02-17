#include "scene/unit.hpp"
#include <gkit.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

using gkit::scene::Unit;

// Test unit class with a test value for verification
class TestUnit : public Unit {
public:
    using Unit::Unit;

    auto _ready() -> void override {}
    auto _process() -> void override {}
    auto _physics_process() -> void override {}
    auto _exit() -> void override {}

    int test_value = 0;
};


// Test 1: Basic iteration over children
auto test_basic_iteration() -> void {
    std::cout << "\n=== Test 1: Basic Iteration ===" << std::endl;

    auto root = Unit::create<Unit>(std::string("root"));

    // Add 5 children
    for (int i = 1; i <= 5; ++i) {
        std::stringstream ss;
        ss << "child" << i;
        auto child = Unit::create<TestUnit>(ss.str());
        child->test_value = i * 10;
        root->add_child(std::move(child));
    }

    // Iterate over children using iterator
    std::cout << "Iterating over children:" << std::endl;
    int count = 0;
    auto view = gkit::scene::iterate_children(root.get());
    for (auto it = view.begin(); it != view.end(); ++it) {
        count++;
        // Use with_child to access TestUnit-specific members
        auto result = root->with_child<TestUnit>(static_cast<uint32_t>(count - 1),
            [](TestUnit& unit) -> int {
                std::cout << "  Child test_value: " << unit.test_value << std::endl;
                return 0;
            });
        if (!result.has_value()) {
            std::cout << "  Child at index " << (count - 1) << " not found" << std::endl;
        }
    }

    if (count == 5) {
        std::cout << "PASS: Found all 5 children" << std::endl;
    } else {
        std::cout << "FAIL: Expected 5 children, found " << count << std::endl;
    }
}


// Test 2: Write access through iterator
auto test_write_access() -> void {
    std::cout << "\n=== Test 2: Write Access ===" << std::endl;

    auto root = Unit::create<Unit>(std::string("root"));

    // Add 3 children
    for (int i = 1; i <= 3; ++i) {
        std::stringstream ss;
        ss << "child" << i;
        auto child = Unit::create<TestUnit>(ss.str());
        child->test_value = 0;
        root->add_child(std::move(child));
    }

    // Modify children through iterator
    std::cout << "Modifying children through iterator:" << std::endl;
    auto view = gkit::scene::iterate_children(root.get());
    int count = 0;
    for (auto it = view.begin(); it != view.end(); ++it) {
        count++;
        // Use with_child to modify TestUnit-specific members
        root->with_child<TestUnit>(static_cast<uint32_t>(count - 1),
            [](TestUnit& unit) -> int {
                unit.test_value = 100;
                std::cout << "  Set child test_value to 100" << std::endl;
                return 0;
            });
    }

    // Verify modifications
    std::cout << "Verifying modifications:" << std::endl;
    bool all_modified = true;
    for (int i = 0; i < count; ++i) {
        auto result = root->with_child<TestUnit>(static_cast<uint32_t>(i),
            [](TestUnit& unit) -> int {
                if (unit.test_value != 100) {
                    std::cout << "  FAIL: test_value is " << unit.test_value << ", expected 100" << std::endl;
                    return 1;
                } else {
                    std::cout << "  OK: test_value is 100" << std::endl;
                    return 0;
                }
            });
        if (result.has_value() && result.value() == 1) {
            all_modified = false;
        }
    }

    if (all_modified) {
        std::cout << "PASS: All children were successfully modified" << std::endl;
    } else {
        std::cout << "FAIL: Not all children were modified" << std::endl;
    }
}


// Test 3: Empty unit (no children)
auto test_empty_unit() -> void {
    std::cout << "\n=== Test 3: Empty Unit ===" << std::endl;

    auto root = Unit::create<Unit>(std::string("root"));

    auto view = gkit::scene::iterate_children(root.get());
    auto begin = view.begin();
    auto end = view.end();

    std::cout << "Iterating over empty unit:" << std::endl;
    int count = 0;
    for (auto it = begin; it != end; ++it) {
        count++;
    }

    if (count == 0) {
        std::cout << "PASS: No children found (as expected)" << std::endl;
    } else {
        std::cout << "FAIL: Expected 0 children, found " << count << std::endl;
    }
}


// Test 4: Iterator with removed children
auto test_removed_children() -> void {
    std::cout << "\n=== Test 4: Removed Children ===" << std::endl;

    auto root = Unit::create<Unit>(std::string("root"));

    // Add 5 children
    for (int i = 1; i <= 5; ++i) {
        std::stringstream ss;
        ss << "child" << i;
        auto child = Unit::create<TestUnit>(ss.str());
        root->add_child(std::move(child));
    }

    // Remove child at index 1 (second child)
    std::cout << "Removing child at index 1..." << std::endl;
    root->remove_child(1);

    // Count remaining children
    auto view = gkit::scene::iterate_children(root.get());
    std::cout << "Iterating over remaining children:" << std::endl;
    int count = 0;
    for (auto it = view.begin(); it != view.end(); ++it) {
        count++;
        std::cout << "  Child at index " << count << std::endl;
    }

    if (count == 4) {
        std::cout << "PASS: Found 4 children after removal" << std::endl;
    } else {
        std::cout << "FAIL: Expected 4 children, found " << count << std::endl;
    }
}


// Test 5: Nested units (multi-level)
auto test_nested_units() -> void {
    std::cout << "\n=== Test 5: Nested Units ===" << std::endl;

    auto root = Unit::create<Unit>(std::string("root"));

    // Add level 1 children
    for (int i = 1; i <= 2; ++i) {
        std::stringstream ss1;
        ss1 << "level1_" << i;
        auto level1 = Unit::create<TestUnit>(ss1.str());

        // Add level 2 children
        for (int j = 1; j <= 2; ++j) {
            std::stringstream ss2;
            ss2 << "level1_" << i << "_level2_" << j;
            auto level2 = Unit::create<TestUnit>(ss2.str());
            level2->test_value = i * 10 + j;
            level1->add_child(std::move(level2));
        }

        root->add_child(std::move(level1));
    }

    // Iterate over root's direct children
    std::cout << "Iterating over root's direct children:" << std::endl;
    int count = 0;
    for (auto it = gkit::scene::iterate_children(root.get()).begin();
         it != gkit::scene::iterate_children(root.get()).end(); ++it) {
        count++;
        std::cout << "  Level 1 child #" << count << std::endl;

        // Iterate over this child's children using with_child
        root->with_child<TestUnit>(static_cast<uint32_t>(count - 1),
            [](TestUnit& unit) -> int {
                auto child_view = gkit::scene::iterate_children(&unit);
                int grandchild_count = 0;
                for (auto child_it = child_view.begin(); child_it != child_view.end(); ++child_it) {
                    grandchild_count++;
                    // Access grandchild through parent's with_child
                    unit.with_child<TestUnit>(static_cast<uint32_t>(grandchild_count - 1),
                        [](TestUnit& gc) -> int {
                            std::cout << "    Grandchild test_value: " << gc.test_value << std::endl;
                            return 0;
                        });
                }
                return 0;
            });
    }

    if (count == 2) {
        std::cout << "PASS: Found 2 level 1 children" << std::endl;
    } else {
        std::cout << "FAIL: Expected 2 level 1 children, found " << count << std::endl;
    }
}


// Test 6: Null unit handling
auto test_null_unit() -> void {
    std::cout << "\n=== Test 6: Null Unit Handling ===" << std::endl;
    
    auto view = gkit::scene::iterate_children(static_cast<Unit*>(nullptr));
    auto begin = view.begin();
    auto end = view.end();
    
    std::cout << "Testing iteration on null unit:" << std::endl;
    
    if (begin == end) {
        std::cout << "PASS: begin() equals end() for null unit" << std::endl;
    } else {
        std::cout << "FAIL: begin() does not equal end() for null unit" << std::endl;
    }
    
    int count = 0;
    for (auto it = begin; it != end; ++it) {
        count++;
    }
    
    if (count == 0) {
        std::cout << "PASS: No iterations on null unit (as expected)" << std::endl;
    } else {
        std::cout << "FAIL: Unexpected iterations on null unit: " << count << std::endl;
    }
}


auto main() -> int {
    std::cout << "========================================" << std::endl;
    std::cout << "Unit Iterator Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    
    test_basic_iteration();
    test_write_access();
    test_empty_unit();
    test_removed_children();
    test_nested_units();
    test_null_unit();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "All tests completed" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}