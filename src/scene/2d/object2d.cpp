/** @file object2d.cpp
 * @brief Construction of the two-dimensional Node specialization.
 */
#include "gkit/scene/2d/object2d.hpp"

#include "gkit/core/reflect/registry.hpp"

namespace gkit::scene {
    auto Object2D::_regist_method(core::reflect::ClassDB& db) -> void {
        db.regist<Object2D>("Object2D", "Node");
    }


    Object2D::Object2D() = default;
} // namespace gkit::scene
