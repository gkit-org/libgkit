#include "register_types.hpp"

#include "gkit/core/reflect/registry.hpp"
#include "gkit/graphic/viewport.hpp"
#include "gkit/graphic/window.hpp"

namespace gkit::graphic {
    auto register_types(core::reflect::ClassDB& db) -> void {
        core::reflect::regist_types<Viewport, Window>(db);
    }
} // namespace gkit::graphic
