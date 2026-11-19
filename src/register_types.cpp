#include "register_types.hpp"

#include "gkit/core/reflect/registry.hpp"

namespace gkit::core::reflect {
    ClassDB::ClassDB() {
        platform::register_types(*this);
        core::register_types(*this);
        math::register_types(*this);
        resource::register_types(*this);
        scene::register_types(*this);
        graphic::register_types(*this);
    }


    auto ClassDB::instance() -> ClassDB& {
        return templates::Singleton<ClassDB>::instance();
    }
} // namespace gkit::core::reflect
