#include "gkit/core/scene/object.hpp"
#include "gkit/core/reflect/registry.hpp"

namespace gkit::core::scene {
    static gkit::core::RegistHolder register_method = []() -> gkit::core::RegistHolder {
        auto& classdb = gkit::core::ClassDB::instance();
        classdb.regist<Object>("Object");
        return {};
    }();
}

