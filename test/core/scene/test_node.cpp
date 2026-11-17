#include "gkit/core/node.hpp"
#include "gkit/core/reflect/registry.hpp"
#include "gkit/scene/2d/object2d.hpp"
#include "test_utils.hpp"

#include <thread>

using namespace gkit::core;
namespace {
    auto check(bool condition) -> void {
        if (!condition) throw std::runtime_error("node regression failed");
    }
    auto test_edits() -> bool {
        auto owner = NodeOwner<>::create();
        auto root  = owner.handle();
        std::vector<NodeHandle<>> handles;
        root.with([&](Node& node) {
            node.edit_children([&](NodeEditor& edit) {
                for (int i = 0; i < 1000; ++i) {
                    auto child = NodeOwner<gkit::scene::Object2D>::create();
                    handles.push_back(child.handle());
                    edit.add(root, "child" + std::to_string(i), std::move(child));
                }
            });
        });
        auto original = root.children();
        auto old_id   = handles[0].id();
        check(original.size() == 1000 && original.at(0)->id() == old_id);
        root.with([&](Node& node) {
            check(node.with_child<gkit::scene::Object2D>("child0", [](auto& child) {
                child.set_rotation(2);
            }));
            check(!node.with_child(1001, [](auto&) {}));
            node.edit_children([&](NodeEditor& edit) {
                for (int i = 0; i < 1000; i += 2) edit.destroy(*original.at(i));
                edit.rename(handles[1], "first");
            });
        });
        check(root.children().size() == 500);
        check(root.children().at(0)->id() == handles[1].id());
        check(root.children().find("first")->id() == handles[1].id());
        check(original.size() == 1000 && original.at(0)->id() == old_id);
        check(!handles[0].with([](auto&) {}) && !old_id.lock());
        std::size_t count = 0;
        root.with([&](Node& node) {
            node.for_each_child([&](auto&) {
                ++count;
            });
        });
        check(count == 500);
        check(root.children().rbegin()->name == "child999");
        return true;
    }
    auto test_atomic_edits_and_ownership() -> bool {
        auto owner       = NodeOwner<>::create();
        auto root        = owner.handle();
        auto child_owner = NodeOwner<>::create();
        auto child       = child_owner.handle();
        auto leaf_owner  = NodeOwner<>::create();
        auto leaf        = leaf_owner.handle();
        child.with([&](Node& node) {
            node.edit_children([&](NodeEditor& e) {
                e.add(child, "leaf", std::move(leaf_owner));
            });
        });
        root.with([&](Node& node) {
            node.edit_children([&](NodeEditor& e) {
                e.add(root, "child", std::move(child_owner));
            });
        });
        auto id = child.id();
        std::optional<NodeOwner<>> detached;
        root.with([&](Node& node) {
            node.edit_children(
                [&](NodeEditor& e) {
                    e.detach(child);
                },
                [&](EditResult result) {
                    check(result.error == NodeError::None && result.owners.size() == 1);
                    detached.emplace(std::move(result.owners[0]));
                });
        });
        check(root.children().empty() && child.id() == id);
        check(child.with([](Node& node) {
            check(!node.parent());
        }));
        check(leaf.with([](auto&) {}));
        root.with([&](Node& node) {
            node.edit_children([&](NodeEditor& e) {
                e.add(root, "child", std::move(*detached));
            });
            node.edit_children(
                [&](NodeEditor& e) {
                    e.reparent(child, leaf);
                },
                [&](EditResult result) {
                    check(result.error == NodeError::InvalidEdit);
                });
            node.edit_children(
                [&](NodeEditor& e) {
                    e.rename(child, "changed");
                    e.add(root, "changed", NodeOwner<>::create());
                },
                [&](EditResult result) {
                    check(result.error == NodeError::InvalidEdit && result.owners.size() == 1);
                    check(result.owners[0].with([](auto&) {}));
                });
        });
        check(root.children().find("child").has_value());
        auto pin     = id.lock<Node>();
        auto leaf_id = leaf.id();
        root.with([&](Node& node) {
            node.edit_children([&](NodeEditor& e) {
                e.destroy(child);
            });
        });
        check(pin != nullptr && !id.lock() && !leaf_id.lock());
        check(!child.with([](auto&) {}) && !leaf.with([](auto&) {}));
        return true;
    }
    auto test_access_and_reflection() -> bool {
        auto owner    = NodeOwner<>::create();
        auto handle   = owner.handle();
        bool rejected = false;
        std::thread other([&] {
            check(handle.children().empty());
            try {
                handle.with([](auto&) {});
            } catch (const std::logic_error&) {
                rejected = true;
            }
        });
        other.join();
        check(rejected);
        owner.with([&](Node& node) {
            auto info = reflect::ClassDB::instance().snapshot("Node");
            check(info && info->set_field(&node, "name", Value(std::string("root"))));
            check(node.name() == "root");
        });
        PublishedState<int> published(1);
        auto old = published.snapshot();
        published.publish(2);
        check(*old == 1 && *published.snapshot() == 2);
        return true;
    }
    auto test_concurrent_snapshots_and_deep_tree() -> bool {
        auto owner                 = NodeOwner<>::create();
        auto root                  = owner.handle();
        std::atomic<bool> finished = false;
        std::thread reader([&] {
            while (!finished.load()) {
                auto snapshot = root.children();
                check(snapshot.size() <= 1);
                if (!snapshot.empty()) check(snapshot.at(0)->id() == snapshot.find("child")->id());
            }
        });
        for (int i = 0; i < 100; ++i)
            root.with([&](Node& node) {
                node.edit_children([&](NodeEditor& edit) {
                    for (const auto& child : node.children()) edit.destroy(child.node);
                    edit.emplace(root, "child");
                });
            });
        finished.store(true);
        reader.join();
        auto subtree = NodeOwner<>::create();
        auto leaf    = subtree.handle();
        for (int i = 0; i < 1000; ++i) {
            auto parent = NodeOwner<>::create();
            parent.with([&](Node& node) {
                node.edit_children([&](NodeEditor& edit) {
                    edit.add(parent.handle(), "next", std::move(subtree));
                });
            });
            subtree = std::move(parent);
        }
        auto leaf_id = leaf.id();
        root.with([&](Node& node) {
            node.edit_children([&](NodeEditor& edit) {
                edit.add(root, "deep", std::move(subtree));
            });
            node.edit_children([&](NodeEditor& edit) {
                edit.destroy(*node.children().find("deep"));
            });
        });
        check(!leaf_id.lock());
        return true;
    }

} // namespace
int main() {
    gkit::test::TestRunner()
        .add_test_func(test_edits)
        .add_test_func(test_atomic_edits_and_ownership)
        .add_test_func(test_access_and_reflection)
        .add_test_func(test_concurrent_snapshots_and_deep_tree)
        .run();
}
