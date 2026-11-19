#pragma once

namespace gkit::core::reflect {
    class ClassDB;
}

namespace gkit::platform {
    auto register_types(core::reflect::ClassDB& db) -> void;
}

namespace gkit::core {
    auto register_types(reflect::ClassDB& db) -> void;
}

namespace gkit::math {
    auto register_types(core::reflect::ClassDB& db) -> void;
}

namespace gkit::resource {
    auto register_types(core::reflect::ClassDB& db) -> void;
}

namespace gkit::scene {
    auto register_types(core::reflect::ClassDB& db) -> void;
}

namespace gkit::graphic {
    auto register_types(core::reflect::ClassDB& db) -> void;
}
