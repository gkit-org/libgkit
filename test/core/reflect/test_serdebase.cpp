#include "gkit/core/unique_object.hpp"
#include "gkit/core/value.hpp"

#include <iostream>

#include <gkit/core/reflect/registry.hpp>
#include <gkit/core/reflect/serdebase.hpp>

using gkit::core::Value;
using gkit::core::reflect::SerdeBase;

class SeralizeObject : public gkit::core::Object {
    int64_t num           = 10;
    float f_num           = 20.f;
    std::string str       = "Hello my girl";
    gkit::core::Array arr = {Value(100), Value(200.f), Value("300")};

    gkit::core::Map map = {{"map_num", Value(100)}, {"map_fnum", Value(200.f)}, {"map_str", Value("str")}};

public:
    static const char* c_name;

    static auto regist_method() -> void {
        auto& db = gkit::core::reflect::ClassDB::instance();
        db.regist<SeralizeObject>(c_name)
            .add_field(c_name, "num", &SeralizeObject::num)
            .add_field(c_name, "f_num", &SeralizeObject::f_num)
            .add_field(c_name, "str", &SeralizeObject::str)
            .add_field(c_name, "arr", &SeralizeObject::arr)
            .add_field(c_name, "map", &SeralizeObject::map);
    }
};

const char* SeralizeObject::c_name = "SeralizeObject";

class Json final : public SerdeBase {
    [[nodiscard]] auto wrapper(gkit::core::Type type, const std::string& key) const -> WrapperPair override {
        std::string begin;
        if (!key.empty()) {
            begin += "\"" + key + "\":";
        }

        std::string end;
        if (type == gkit::core::Type::Array) {
            begin += "[";
            end = "]";
        } else if (type == gkit::core::Type::Map || type == gkit::core::Type::Object) {
            begin += "{";
            end = "}";
        }

        return {begin, end};
    }


    [[nodiscard]] auto element_gap(gkit::core::Type /*type*/) const -> std::string override { return ","; }


    [[nodiscard]] auto leaf_value(const Value& v) const -> std::string override {
        switch (v.type()) {
        case gkit::core::Type::String: {
            return "\"" + v.as_string() + "\"";
        }
        case gkit::core::Type::Bool: {
            return v.as_bool() ? "true" : "false";
        }
        case gkit::core::Type::Number: {
            return v.is_number_float() ? std::to_string(v.as_float()) : std::to_string(v.as_int64());
        }
        case gkit::core::Type::Null: {
            return "null";
        }
        default: {
            return {};
        }
        }
    }
};

auto main() -> int {
    SeralizeObject::regist_method();
    auto json = Json();
    auto obj  = gkit::core::UniqueObject::create<SeralizeObject>();
    json.from(obj.get_id());
    auto serde_str = json.to_string();
    std::cout << serde_str << '\n';

    const auto expect =
        R"({"num":10,"f_num":20.000000,"str":"Hello my girl","arr":[100,200.000000,"300"],"map":{"map_fnum":200.000000,"map_num":100,"map_str":"str"}})";
    if (serde_str != expect) {
        std::cerr << "unexpected output:\n  got:      " << serde_str << '\n' << "  expected: " << expect << '\n';
        return 1;
    }

    // ObjectId values do not participate in (de)serialization for now.
    auto obj2 = gkit::core::UniqueObject::create<SeralizeObject>();
    json.from(Value(obj2.get_id()));
    const auto ref_str = json.to_string();
    if (!ref_str.empty()) {
        std::cerr << "expected ObjectId to be skipped, got: " << ref_str << '\n';
        return 1;
    }

    const gkit::core::Array mixed = {Value(1), Value(obj2.get_id()), Value(2)};
    json.from(Value(mixed));
    const auto mixed_str = json.to_string();
    if (mixed_str != "[1,2]") {
        std::cerr << "unexpected mixed array output: " << mixed_str << '\n';
        return 1;
    }

    return 0;
}