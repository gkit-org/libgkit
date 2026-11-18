#include "gkit/core/reflect/registry.hpp"
#include "gkit/scene/2d/object2d.hpp"
#include "gkit/scene/scene.hpp"
#include "test_utils.hpp"

#include <barrier>
#include <thread>

using namespace gkit::core;
namespace {
    void check(bool ok) {
        if (!ok) throw std::runtime_error("node regression failed");
    }
    auto test_edits_and_ownership() -> bool {
        auto owner = UniqueObject::create<Node>();
        auto& root = *static_cast<Node*>(owner.get());
        auto child = UniqueObject::create<Node>();
        auto id    = child.get_id();
        root.add_child("child", std::move(child));
        root.add_child("sibling", UniqueObject::create<Node>());
        auto old     = root.children();
        auto sibling = *root.find_child("sibling");
        check(old.size() == 2 && old[0].id == id);
        check(id.with<Node>([&](Node& node) {
            node.rename("first");
            node.rename("sibling", [](EditResult result) {
                check(result.error == NodeError::InvalidEdit);
            });
            check(root.find_child("first") == id && old[0].name == "child");
            node.add_child("leaf", UniqueObject::create<Node>());
            node.reparent(*node.find_child("leaf"), [](EditResult result) {
                check(result.error == NodeError::InvalidEdit);
            });
            check(node.parent() == root.id());
            node.reparent(sibling);
            check(node.parent() == sibling);
        }));
        std::optional<UniqueObject> detached;
        root.detach_child(sibling, [&](EditResult result) {
            detached = std::move(result.owner);
        });
        check(root.children().empty() && detached && detached->get_id() == sibling);
        root.add_child("sibling", std::move(*detached));
        root.add_child("sibling", UniqueObject::create<Node>(), [&](EditResult result) {
            check(result.error == NodeError::InvalidEdit && result.owner && root.find_child("sibling") == sibling);
        });
        check(sibling.with<Node>([&](Node& pinned) {
            root.remove_child(sibling);
            check(!sibling.with([](Object&) {}) && !id.with([](Object&) {}));
            check(pinned.children().empty());
        }));
        return true;
    }

    auto test_nearest_ancestor() -> bool {
        using gkit::scene::Scene;
        struct Level : Scene {};
        auto outer          = UniqueObject::create<Scene>();
        auto inner          = UniqueObject::create<Level>();
        auto group          = UniqueObject::create<Node>();
        auto leaf           = UniqueObject::create<Node>();
        const auto inner_id = inner.get_id(), group_id = group.get_id(), leaf_id = leaf.get_id();
        auto& root = *static_cast<Scene*>(outer.get());
        static_cast<Node*>(group.get())->add_child("leaf", std::move(leaf));
        static_cast<Scene*>(inner.get())->add_child("group", std::move(group));
        root.add_child("level", std::move(inner));

        check(!root.find_ancestor<Scene>());
        check(leaf_id.with<Node>([&](Node& node) {
            check(node.find_ancestor<Node>() == group_id);
            check(node.find_ancestor<Scene>() == inner_id);
            check(!node.find_ancestor<gkit::scene::Object2D>());
            node.reparent(root.id());
            check(node.find_ancestor<Scene>() == root.id());
            check(!node.find_ancestor<Level>());
        }));
        return true;
    }

    auto test_access_and_reflection() -> bool {
        auto owner = UniqueObject::create<gkit::scene::Object2D>();
        auto id    = owner.get_id();
        auto info  = reflect::ClassDB::instance().find("Node");
        check(info->set_field(owner.get(), "name", Value(std::string("root"))));
        check(!id.with<gkit::scene::Scene>([](auto&) {}));
        auto scene = UniqueObject::create<gkit::scene::Scene>();
        auto& gate = *static_cast<gkit::scene::Scene*>(scene.get());
        std::barrier start(2);
        std::thread writer([&] {
            start.arrive_and_wait();
            id.with<gkit::scene::Object2D>([&](auto& node) {
                for (int i = 1; i <= 500; ++i) {
                    node.set_position({float(i), float(i)});
                    node.set_enabled(i % 2);
                    gate.pause();
                    gate.resume();
                }
            });
        });
        start.arrive_and_wait();
        check(id.with<gkit::scene::Object2D>([&](auto& node) {
            check(node.name() == "root");
            for (int i = 0; i < 500; ++i) {
                auto pos = node.get_transform().get_position();
                check(pos.x == pos.y);
                (void)node.updates_enabled();
                (void)gate.updates_enabled();
            }
        }));
        writer.join();
        return true;
    }

    auto test_concurrent_snapshots_and_deep_tree() -> bool {
        auto owner = UniqueObject::create<Node>();
        auto& root = *static_cast<Node*>(owner.get());
        std::barrier start(2);
        std::thread reader([&] {
            start.arrive_and_wait();
            for (int i = 0; i < 1000; ++i) {
                auto snapshot = root.children();
                check(snapshot.size() <= 1);
                if (!snapshot.empty()) {
                    check(snapshot[0].name == "child" || snapshot[0].name == "renamed");
                    snapshot[0].id.with<Node>([](Node& node) {
                        (void)node.name();
                    });
                }
            }
        });
        start.arrive_and_wait();
        for (int i = 0; i < 500; ++i) {
            root.add_child("child", UniqueObject::create<Node>());
            auto child = *root.find_child("child");
            child.with<Node>([](Node& node) {
                node.rename("renamed");
            });
            root.remove_child(child);
        }
        reader.join();
        auto subtree = UniqueObject::create<Node>();
        auto leaf    = subtree.get_id();
        for (int i = 0; i < 1000; ++i) {
            auto parent = UniqueObject::create<Node>();
            static_cast<Node*>(parent.get())->add_child("next", std::move(subtree));
            subtree = std::move(parent);
        }
        root.add_child("deep", std::move(subtree));
        root.remove_child(*root.find_child("deep"));
        return !leaf.with([](Object&) {});
    }
} // namespace
int main() {
    gkit::test::TestRunner()
        .add_test_func(test_edits_and_ownership)
        .add_test_func(test_nearest_ancestor)
        .add_test_func(test_access_and_reflection)
        .add_test_func(test_concurrent_snapshots_and_deep_tree)
        .run();
}
