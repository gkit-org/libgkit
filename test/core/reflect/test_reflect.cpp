#include "gkit/core/reflect/registry.hpp"
#include "gkit/core/scene/object.hpp"

#include <cassert>
#include <iostream>
#include <string>

using gkit::core::ClassDB;
using gkit::core::ClassInfo;
using gkit::core::FieldDesc;
using gkit::core::Value;
using gkit::core::scene::Object;

// =========================================================================
// Test types
// =========================================================================

class ReflectTestObj : public Object {
public:
    bool active       = true;
    int count         = 0;
    float scale       = 1.0f;
    std::string label = "default";
};

class ReadOnlyObj : public Object {
    std::string secret = "hidden";

public:
    [[nodiscard]] auto get_secret() const -> const std::string& { return secret; }
};

class ChildObj : public ReflectTestObj {
public:
    float weight = 0.0;
};

// =========================================================================
// Registration
// =========================================================================

auto init_reflect() -> void {
    auto& db = ClassDB::instance();

    db.regist<ReflectTestObj>("ReflectTestObj")
        .add_field<ReflectTestObj>("ReflectTestObj", "active", &ReflectTestObj::active)
        .add_field<ReflectTestObj>("ReflectTestObj", "count", &ReflectTestObj::count)
        .add_field<ReflectTestObj>("ReflectTestObj", "scale", &ReflectTestObj::scale)
        .add_field<ReflectTestObj>("ReflectTestObj", "label", &ReflectTestObj::label);

    db.regist<ReadOnlyObj>("ReadOnlyObj")
        .add_property<ReadOnlyObj>("ReadOnlyObj", "secret", [](const ReadOnlyObj& o) -> Value {
            return Value(o.get_secret());
        });

    db.regist<ChildObj>("ChildObj", "ReflectTestObj").add_field<ChildObj>("ChildObj", "weight", &ChildObj::weight);
}

// =========================================================================
// Tests
// =========================================================================

auto test_regist_and_find() -> bool {
    std::cout << "=== Test: regist and find ===" << '\n';

    auto* info = ClassDB::instance().find("ReflectTestObj");
    assert(info != nullptr);
    assert(info->class_name == "ReflectTestObj");
    std::cout << "  find existing class: OK" << '\n';

    auto* missing = ClassDB::instance().find("NonExistent");
    assert(missing == nullptr);
    std::cout << "  find missing class returns nullptr: OK" << '\n';

    return true;
}

auto test_add_field() -> bool {
    std::cout << "=== Test: add_field ===" << '\n';

    auto* info = ClassDB::instance().find("ReflectTestObj");
    assert(info != nullptr);
    assert(info->fields.size() == 4);
    assert(info->fields[0].name == "active");
    assert(info->fields[1].name == "count");
    assert(info->fields[2].name == "scale");
    assert(info->fields[3].name == "label");
    std::cout << "  fields registered: OK" << '\n';

    return true;
}

auto test_get_field() -> bool {
    std::cout << "=== Test: get_field ===" << '\n';

    ReflectTestObj obj;
    auto* info = ClassDB::instance().find("ReflectTestObj");

    auto active_v = info->get_field(&obj, "active");
    assert(active_v.has_value());
    assert(active_v->is_bool());
    assert(active_v->as_bool() == true);
    std::cout << "  get bool field: OK" << '\n';

    auto count_v = info->get_field(&obj, "count");
    assert(count_v.has_value());
    assert(count_v->is_number_integer());
    assert(count_v->as_int64() == 0);
    std::cout << "  get int field: OK" << '\n';

    auto scale_v = info->get_field(&obj, "scale");
    assert(scale_v.has_value());
    assert(scale_v->as_float() == 1.0);
    std::cout << "  get float field: OK" << '\n';

    auto label_v = info->get_field(&obj, "label");
    assert(label_v.has_value());
    assert(label_v->is_string());
    assert(label_v->as_string() == "default");
    std::cout << "  get string field: OK" << '\n';

    auto missing_v = info->get_field(&obj, "nonexistent");
    assert(!missing_v.has_value());
    std::cout << "  get missing field returns nullopt: OK" << '\n';

    return true;
}

auto test_set_field() -> bool {
    std::cout << "=== Test: set_field ===" << '\n';

    ReflectTestObj obj;
    auto* info = ClassDB::instance().find("ReflectTestObj");

    auto ok = info->set_field(&obj, "active", Value(false));
    assert(ok);
    assert(obj.active == false);
    assert(info->get_field(&obj, "active")->as_bool() == false);
    std::cout << "  set bool field: OK" << '\n';

    ok = info->set_field(&obj, "count", Value(42));
    assert(ok);
    assert(obj.count == 42);
    std::cout << "  set int field: OK" << '\n';

    ok = info->set_field(&obj, "scale", Value(2.5f));
    assert(ok);
    assert(obj.scale == 2.5f);
    std::cout << "  set float field: OK" << '\n';

    ok = info->set_field(&obj, "label", Value(std::string("updated")));
    assert(ok);
    assert(obj.label == "updated");
    std::cout << "  set string field: OK" << '\n';

    ok = info->set_field(&obj, "nonexistent", Value(0));
    assert(!ok);
    std::cout << "  set missing field returns false: OK" << '\n';

    return true;
}

auto test_readonly_property() -> bool {
    std::cout << "=== Test: read-only property ===" << '\n';

    ReadOnlyObj obj;
    auto* info = ClassDB::instance().find("ReadOnlyObj");

    assert(info != nullptr);
    assert(info->fields.size() == 1);
    assert(info->fields[0].name == "secret");
    std::cout << "  field registered: OK" << '\n';

    auto secret_v = info->get_field(&obj, "secret");
    assert(secret_v.has_value());
    assert(secret_v->is_string());
    assert(secret_v->as_string() == "hidden");
    std::cout << "  get read-only field: OK" << '\n';

    auto ok = info->set_field(&obj, "secret", Value(std::string("attempt")));
    assert(!ok);
    std::cout << "  set read-only field returns false: OK" << '\n';

    return true;
}

auto test_parent_inheritance() -> bool {
    std::cout << "=== Test: parent inheritance ===" << '\n';

    auto* info = ClassDB::instance().find("ChildObj");
    assert(info != nullptr);

    auto* p = info->parent();
    assert(p != nullptr);
    assert(p->class_name == "ReflectTestObj");
    std::cout << "  parent() resolves correctly: OK" << '\n';

    auto* root = ClassDB::instance().find("ReflectTestObj");
    assert(root->parent() == nullptr);
    std::cout << "  root class has no parent: OK" << '\n';

    return true;
}

auto test_for_each_field_inheritance() -> bool {
    std::cout << "=== Test: for_each_field with inheritance ===" << '\n';

    auto* info = ClassDB::instance().find("ChildObj");
    assert(info != nullptr);

    std::vector<std::string> field_names;
    info->for_each_field([&](const FieldDesc& f, auto& /*getter*/, const auto* /*setter*/) {
        field_names.push_back(f.name);
    });

    assert(field_names.size() == 5);
    assert(field_names[0] == "active");
    assert(field_names[1] == "count");
    assert(field_names[2] == "scale");
    assert(field_names[3] == "label");
    assert(field_names[4] == "weight");
    std::cout << "  parent fields come before child fields: OK" << '\n';

    return true;
}

auto test_get_field_inherited() -> bool {
    std::cout << "=== Test: get_field inherited ===" << '\n';

    ChildObj obj;
    obj.label  = "child_label";
    obj.weight = 3.14;

    auto* info = ClassDB::instance().find("ChildObj");

    auto label_v = info->get_field(&obj, "label");
    assert(label_v.has_value());
    assert(label_v->as_string() == "child_label");
    std::cout << "  get inherited field: OK" << '\n';

    auto weight_v = info->get_field(&obj, "weight");
    assert(weight_v.has_value());
    assert(weight_v->as_float() == 3.14f);
    std::cout << "  get own field: OK" << '\n';

    return true;
}

auto test_set_field_inherited() -> bool {
    std::cout << "=== Test: set_field inherited ===" << '\n';

    ChildObj obj;
    auto* info = ClassDB::instance().find("ChildObj");

    auto ok = info->set_field(&obj, "active", Value(false));
    assert(ok);
    assert(obj.active == false);
    std::cout << "  set inherited field: OK" << '\n';

    ok = info->set_field(&obj, "weight", Value(1.618f));
    assert(ok);
    assert(obj.weight == 1.618f);
    std::cout << "  set own field: OK" << '\n';

    return true;
}

// =========================================================================
// Entry
// =========================================================================

auto main() -> int {
    init_reflect();

    auto ok = true;
    ok      &= test_regist_and_find();
    ok      &= test_add_field();
    ok      &= test_get_field();
    ok      &= test_set_field();
    ok      &= test_readonly_property();
    ok      &= test_parent_inheritance();
    ok      &= test_for_each_field_inheritance();
    ok      &= test_get_field_inherited();
    ok      &= test_set_field_inherited();

    if (ok) {
        std::cout << '\n' << "All tests passed." << '\n';
        return 0;
    }
    std::cerr << '\n' << "Some tests failed." << '\n';
    return 1;
}
