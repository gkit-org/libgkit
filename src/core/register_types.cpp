#include "register_types.hpp"

#include "gkit/core/node.hpp"
#include "gkit/core/object.hpp"
#include "gkit/core/reflect/registry.hpp"

namespace gkit::core {
    auto register_types(reflect::ClassDB& db) -> void {
        reflect::regist_types<Object, Node>(db);
    }
} // namespace gkit::core
