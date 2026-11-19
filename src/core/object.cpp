#include "gkit/core/object.hpp"

#include "gkit/core/reflect/registry.hpp"

namespace gkit::core {
    auto Object::_regist_method(reflect::ClassDB& db) -> void {
        db.regist<Object>("Object");
    }


    auto Object::class_name() const -> std::string {
        auto* raw_name  = typeid(*this).name();
        auto class_info = reflect::ClassDB::instance().find_with_raw(raw_name);
        if (!class_info) throw std::invalid_argument("object type is not registered");
        return class_info->class_name;
    }
} // namespace gkit::core
