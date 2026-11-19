/** @file sprite2d_reflection.cpp
 * @brief Register Sprite2D independently of the image decoder for static-library consumers.
 */
#include "gkit/scene/2d/sprite2d.hpp"

#include "gkit/core/reflect/registry.hpp"

namespace gkit::scene {
    auto Sprite2D::_regist_method(core::reflect::ClassDB& db) -> void {
        db.regist<Sprite2D>("Sprite2D", "Object2D");
    }
} // namespace gkit::scene
