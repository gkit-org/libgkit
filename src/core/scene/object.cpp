#include "gkit/core/scene/object.hpp"
#include "gkit/core/reflect/registry.hpp"

namespace gkit::core::scene {
    static auto regist_holder = gkit::core::RegistHolder ([]() {
        auto& classdb = gkit::core::ClassDB::instance();
        classdb.regist<Object>("Object");
        return;
    });
}
