#include "gkit/graphic/viewport.hpp"

#include "gkit/core/reflect/registry.hpp"

namespace gkit::graphic {
    auto Viewport::_regist_method(core::reflect::ClassDB& db) -> void {
        db.regist<Viewport>("Viewport", "Node");
    }
} // namespace gkit::graphic
