/** @file scene.cpp
 * @brief Reflection registration for composable scene state.
 */
#include "gkit/scene/scene.hpp"

#include "gkit/core/reflect/registry.hpp"

namespace gkit::scene {
    Scene::Scene() {
        static const bool registered = [] {
            core::reflect::ClassDB::instance().register_class<Scene>(
                "Scene",
                [](auto& db) {
                    db.template add_property<Scene>(
                        "Scene",
                        "active",
                        [](const Scene& scene) {
                            return core::Value(scene.is_active());
                        },
                        [](Scene& scene, const core::Value& value) {
                            if (value.as_bool())
                                scene.activate();
                            else
                                scene.deactivate();
                        });
                    db.template add_property<Scene>(
                        "Scene",
                        "paused",
                        [](const Scene& scene) {
                            return core::Value(scene.is_paused());
                        },
                        [](Scene& scene, const core::Value& value) {
                            if (value.as_bool())
                                scene.pause();
                            else
                                scene.resume();
                        });
                },
                "Node");
            return true;
        }();
        (void)registered;
    }
} // namespace gkit::scene
