#include "gkit/core/node.hpp"
#include "gkit/core/reflect/registry.hpp"
#include "gkit/graphic/viewport.hpp"
#include "gkit/graphic/window.hpp"
#include "gkit/scene/2d/object2d.hpp"
#include "gkit/scene/2d/sprite2d.hpp"
#include "gkit/scene/scene.hpp"

namespace gkit::core::reflect {
    // Register parents before their children, even when a consumer calls instance() from a static initializer.
    static auto register_builtin_types() -> ClassDB& {
        static auto& registry = []() -> ClassDB& {
            auto& db = templates::Singleton<ClassDB>::instance();
            db.regist<Object>("Object");
            db.register_class<Node>("Node", [](ClassDB& staging) {
                staging.add_property<Node>(
                    "Node",
                    "name",
                    [](const Node& node) {
                        return Value(node.name());
                    },
                    [](Node& node, const Value& value) {
                        node.rename(value.as_string());
                    });
            });
            db.register_class<scene::Scene>(
                "Scene",
                [](ClassDB& staging) {
                    staging.add_property<scene::Scene>(
                        "Scene",
                        "active",
                        [](const scene::Scene& scene) {
                            return Value(scene.is_active());
                        },
                        [](scene::Scene& scene, const Value& value) {
                            if (value.as_bool())
                                scene.activate();
                            else
                                scene.deactivate();
                        });
                    staging.add_property<scene::Scene>(
                        "Scene",
                        "paused",
                        [](const scene::Scene& scene) {
                            return Value(scene.is_paused());
                        },
                        [](scene::Scene& scene, const Value& value) {
                            if (value.as_bool())
                                scene.pause();
                            else
                                scene.resume();
                        });
                },
                "Node");
            db.regist<scene::Object2D>("Object2D", "Node");
            db.regist<scene::Sprite2D>("Sprite2D", "Object2D");
            db.regist<graphic::Viewport>("Viewport", "Node");
            db.regist<graphic::Window>("Window", "Viewport");
            return db;
        }();
        return registry;
    }

    static const RegistHolder regist_holder([] {
        register_builtin_types();
    });

    auto ClassDB::instance() -> ClassDB& {
        return register_builtin_types();
    }
} // namespace gkit::core::reflect
