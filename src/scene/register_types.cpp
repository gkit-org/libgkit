#include "register_types.hpp"

#include "gkit/core/reflect/registry.hpp"
#include "gkit/scene/2d/object2d.hpp"
#include "gkit/scene/2d/sprite2d.hpp"
#include "gkit/scene/scene.hpp"

namespace gkit::scene {
    auto register_types(core::reflect::ClassDB& db) -> void {
        core::reflect::regist_types<Scene, Object2D, Sprite2D>(db);
    }
} // namespace gkit::scene
