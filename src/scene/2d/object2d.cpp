/** @file object2d.cpp
 * @brief Registration of the two-dimensional Node specialization.
 */
#include "gkit/scene/2d/object2d.hpp"

#include "gkit/core/reflect/registry.hpp"

namespace gkit::scene {
    Object2D::Object2D() {
        static const bool registered = [] {
            core::reflect::ClassDB::instance().register_class<Object2D>("Object2D", [](auto&) {}, "Node");
            return true;
        }();
        (void)registered;
    }
} // namespace gkit::scene
