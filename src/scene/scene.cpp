/** @file scene.cpp
 * @brief Construction of a composable scene.
 */
#include "gkit/scene/scene.hpp"

#include "gkit/core/reflect/registry.hpp"

namespace gkit::scene {
    auto Scene::_regist_method(core::reflect::ClassDB& db) -> void {
        db.regist<Scene>("Scene", "Node")
            .add_property("Scene", "active", &Scene::get_active, &Scene::set_active)
            .add_property("Scene", "paused", &Scene::get_paused, &Scene::set_paused);
    }


    Scene::Scene() = default;
} // namespace gkit::scene
