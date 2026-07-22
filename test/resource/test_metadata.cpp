#include <cassert>
#include <iostream>
#include <map>
#include <string>

#include <gkit/resource/metadata.hpp>

using gkit::resource::metadata::Array;
using gkit::resource::metadata::Object;
using gkit::resource::metadata::parse;
using gkit::resource::metadata::ParseError;
using gkit::resource::metadata::serialize;
using gkit::resource::metadata::serialize_pretty;
using gkit::resource::metadata::try_parse;
using gkit::resource::metadata::Value;

auto test_value_construction() -> void {
    std::cout << "=== Test: Value Construction ===" << '\n';

    // Null
    Value v_null;
    assert(v_null.is_null());
    std::cout << "  Null: OK" << '\n';

    // Bool
    Value v_true(true);
    Value v_false(false);
    assert(v_true.is_bool() && v_true.as_bool() == true);
    assert(v_false.is_bool() && v_false.as_bool() == false);
    std::cout << "  Bool: OK" << '\n';

    // Integer
    Value v_int(42);
    assert(v_int.is_number() && v_int.is_number_integer());
    assert(v_int.as_int64() == 42);
    std::cout << "  Integer: OK" << '\n';

    // Double
    Value v_double(3.14159);
    assert(v_double.is_number() && v_double.is_number_float());
    assert(v_double.as_double() > 3.14 && v_double.as_double() < 3.15);
    std::cout << "  Double: OK" << '\n';

    // String
    Value v_str("hello");
    Value v_str2(std::string("world"));
    assert(v_str.is_string() && v_str.as_string() == "hello");
    assert(v_str2.is_string() && v_str2.as_string() == "world");
    std::cout << "  String: OK" << '\n';

    // Array
    Array arr;
    arr.push_back(Value(1));
    arr.push_back(Value(2));
    Value v_arr(std::move(arr));
    assert(v_arr.is_array());
    assert(v_arr.as_array().size() == 2);
    std::cout << "  Array: OK" << '\n';

    // Object
    Object obj;
    obj["key"] = Value("value");
    Value v_obj(std::move(obj));
    assert(v_obj.is_object());
    assert(v_obj.contains("key"));
    std::cout << "  Object: OK" << '\n';

    std::cout << "All construction tests passed!" << '\n' << '\n';
}

auto test_parsing_literals() -> void {
    std::cout << "=== Test: Parsing Literals ===" << '\n';

    // Null
    auto null_val = parse("null");
    assert(null_val.is_null());
    std::cout << "  null: OK" << '\n';

    // Booleans
    auto true_val  = parse("true");
    auto false_val = parse("false");
    assert(true_val.is_bool() && true_val.as_bool() == true);
    assert(false_val.is_bool() && false_val.as_bool() == false);
    std::cout << "  true/false: OK" << '\n';

    std::cout << "All literal parsing tests passed!" << '\n' << '\n';
}

auto test_parsing_numbers() -> void {
    std::cout << "=== Test: Parsing Numbers ===" << '\n';

    // Integers
    assert(parse("0").as_int64() == 0);
    assert(parse("42").as_int64() == 42);
    assert(parse("-17").as_int64() == -17);
    assert(parse("123456789").as_int64() == 123456789);
    std::cout << "  Integers: OK" << '\n';

    // Floats
    assert(parse("3.14").is_number_float());
    assert(parse("-0.5").as_double() == -0.5);
    assert(parse("1.0").as_double() == 1.0);
    std::cout << "  Floats: OK" << '\n';

    // Scientific notation
    assert(parse("1e10").is_number_float());
    assert(parse("1.5e-3").as_double() == 0.0015);
    assert(parse("-2E+5").as_double() == -200000.0);
    std::cout << "  Scientific: OK" << '\n';

    std::cout << "All number parsing tests passed!" << '\n' << '\n';
}

auto test_parsing_strings() -> void {
    std::cout << "=== Test: Parsing Strings ===" << '\n';

    // Basic strings
    assert(parse("\"hello\"").as_string() == "hello");
    assert(parse("\"\"").as_string() == "");
    std::cout << "  Basic strings: OK" << '\n';

    // Escapes
    assert(parse("\"hello\\nworld\"").as_string() == "hello\nworld");
    assert(parse("\"tab\\there\"").as_string() == "tab\there");
    assert(parse("\"quote\\\"inside\"").as_string() == "quote\"inside");
    assert(parse("\"backslash\\\\here\"").as_string() == "backslash\\here");
    assert(parse("\"\\r\\n\\t\\b\\f\"").as_string() == "\r\n\t\b\f");
    std::cout << "  Escapes: OK" << '\n';

    // Unicode escapes
    assert(parse("\"\\u0041\"").as_string() == "A");
    assert(parse("\"\\u00e9\"").as_string() == "\xc3\xa9"); // é
    std::cout << "  Unicode: OK" << '\n';

    std::cout << "All string parsing tests passed!" << '\n' << '\n';
}

auto test_parsing_arrays() -> void {
    std::cout << "=== Test: Parsing Arrays ===" << '\n';

    // Empty array
    auto empty = parse("[]");
    assert(empty.is_array() && empty.as_array().empty());
    std::cout << "  Empty array: OK" << '\n';

    // Single element
    auto single = parse("[42]");
    assert(single[0].as_int64() == 42);
    std::cout << "  Single element: OK" << '\n';

    // Multiple elements
    auto multi = parse("[1, 2, 3]");
    assert(multi[0].as_int64() == 1);
    assert(multi[1].as_int64() == 2);
    assert(multi[2].as_int64() == 3);
    std::cout << "  Multiple elements: OK" << '\n';

    // Mixed types
    auto mixed = parse("[1, \"two\", true, null]");
    assert(mixed[0].as_int64() == 1);
    assert(mixed[1].as_string() == "two");
    assert(mixed[2].as_bool() == true);
    assert(mixed[3].is_null());
    std::cout << "  Mixed types: OK" << '\n';

    // Nested arrays
    auto nested = parse("[[1, 2], [3, 4]]");
    assert(nested[0][0].as_int64() == 1);
    assert(nested[1][1].as_int64() == 4);
    std::cout << "  Nested arrays: OK" << '\n';

    std::cout << "All array parsing tests passed!" << '\n' << '\n';
}

auto test_parsing_objects() -> void {
    std::cout << "=== Test: Parsing Objects ===" << '\n';

    // Empty object
    auto empty = parse("{}");
    assert(empty.is_object() && empty.as_object().empty());
    std::cout << "  Empty object: OK" << '\n';

    // Single key
    auto single = parse("{\"name\": \"test\"}");
    assert(single["name"].as_string() == "test");
    std::cout << "  Single key: OK" << '\n';

    // Multiple keys
    auto multi = parse(R"({"x": 10, "y": 20, "z": 30})");
    assert(multi["x"].as_int64() == 10);
    assert(multi["y"].as_int64() == 20);
    assert(multi["z"].as_int64() == 30);
    std::cout << "  Multiple keys: OK" << '\n';

    // Nested objects
    auto nested = parse(R"({"user": {"name": "John", "age": 30}})");
    assert(nested["user"]["name"].as_string() == "John");
    assert(nested["user"]["age"].as_int64() == 30);
    std::cout << "  Nested objects: OK" << '\n';

    // Array in object
    auto arr_in_obj = parse(R"({"items": [1, 2, 3]})");
    assert(arr_in_obj["items"][1].as_int64() == 2);
    std::cout << "  Array in object: OK" << '\n';

    // Object in array
    auto obj_in_arr = parse(R"([{"id": 1}, {"id": 2}])");
    assert(obj_in_arr[0]["id"].as_int64() == 1);
    assert(obj_in_arr[1]["id"].as_int64() == 2);
    std::cout << "  Object in array: OK" << '\n';

    std::cout << "All object parsing tests passed!" << '\n' << '\n';
}

auto test_serialization() -> void {
    std::cout << "=== Test: Serialization ===" << '\n';

    // Compact serialization
    auto compact = serialize(parse(R"({"a": 1, "b": [true, null]})"));
    assert(compact.find('\n') == std::string::npos);
    std::cout << "  Compact: " << compact << '\n';
    std::cout << "  Compact serialization: OK" << '\n';

    // Pretty serialization
    auto pretty = serialize_pretty(parse(R"({"x": 1, "y": 2})"));
    std::cout << "  Pretty:\n" << pretty << '\n';
    assert(pretty.find('\n') != std::string::npos);
    std::cout << "  Pretty serialization: OK" << '\n';

    // Round-trip test
    auto original   = R"({"name": "test", "values": [1, 2, 3], "nested": {"a": true}})";
    auto parsed     = parse(original);
    auto serialized = serialize(parsed);
    auto reparsed   = parse(serialized);
    assert(reparsed["name"].as_string() == "test");
    assert(reparsed["values"][2].as_int64() == 3);
    assert(reparsed["nested"]["a"].as_bool() == true);
    std::cout << "  Round-trip: OK" << '\n';

    std::cout << "All serialization tests passed!" << '\n' << '\n';
}

auto test_accessors() -> void {
    std::cout << "=== Test: Accessors ===" << '\n';

    Value v = parse(R"({"num": 42, "str": "hello", "flag": true})");

    // Safe at() accessor
    auto num_opt = v.at("num");
    assert(num_opt.has_value());
    assert(num_opt->get().as_int64() == 42);
    std::cout << "  at() for existing key: OK" << '\n';

    auto missing = v.at("missing");
    assert(!missing.has_value());
    std::cout << "  at() for missing key: OK" << '\n';

    // Fallback accessors
    Value wrong_type("string");
    assert(wrong_type.as_int64_or(100) == 100);
    assert(wrong_type.as_double_or(3.14) == 3.14);
    assert(wrong_type.as_bool_or(true) == true);
    assert(wrong_type.as_string_or("fallback") == "string");
    std::cout << "  Fallback accessors: OK" << '\n';

    // Contains
    assert(v.contains("num"));
    assert(!v.contains("nonexistent"));
    std::cout << "  contains(): OK" << '\n';

    std::cout << "All accessor tests passed!" << '\n' << '\n';
}

auto test_try_parse() -> void {
    std::cout << "=== Test: try_parse (non-throwing) ===" << '\n';

    auto valid = try_parse(R"({"valid": true})");
    assert(valid.has_value());
    assert(valid->is_object());
    std::cout << "  Valid JSON: OK" << '\n';

    auto invalid = try_parse(R"({"invalid": })");
    assert(!invalid.has_value());
    std::cout << "  Invalid JSON returns nullopt: OK" << '\n';

    auto garbage = try_parse("not json at all");
    assert(!garbage.has_value());
    std::cout << "  Garbage input returns nullopt: OK" << '\n';

    std::cout << "All try_parse tests passed!" << '\n' << '\n';
}

auto test_parse_errors() -> void {
    std::cout << "=== Test: Parse Errors ===" << '\n';

    // Missing closing brace
    try {
        (void)parse("{");
        assert(false); // Should not reach here
    } catch (const ParseError& e) {
        std::cout << "  Missing brace: " << e.what() << '\n';
    }

    // Invalid escape
    try {
        (void)parse(R"("\x")");
        assert(false);
    } catch (const ParseError& e) {
        std::cout << "  Invalid escape: " << e.what() << '\n';
    }

    // Trailing comma
    try {
        (void)parse("[1, 2,]");
        assert(false);
    } catch (const ParseError& e) {
        std::cout << "  Trailing comma: " << e.what() << '\n';
    }

    // Invalid number
    try {
        (void)parse("01");
        assert(false);
    } catch (const ParseError& e) {
        std::cout << "  Leading zero: " << e.what() << '\n';
    }

    std::cout << "All parse error tests passed!" << '\n' << '\n';
}

auto test_complex_example() -> void {
    std::cout << "=== Test: Complex Real-World Example ===" << '\n';

    const char* config_json = R"({
        "application": {
            "name": "MyGame",
            "version": "1.0.0",
            "fullscreen": false,
            "resolution": {
                "width": 1920,
                "height": 1080
            }
        },
        "graphics": {
            "vsync": true,
            "msaa": 4,
            "shadow_quality": "high"
        },
        "audio": {
            "master_volume": 0.8,
            "music_volume": 0.6,
            "sfx_volume": 1.0
        },
        "controls": [
            {"action": "jump", "key": "Space"},
            {"action": "attack", "key": "MouseLeft"},
            {"action": "inventory", "key": "Tab"}
        ]
    })";

    auto config = parse(config_json);

    // Navigate nested structure
    assert(config["application"]["name"].as_string() == "MyGame");
    assert(config["application"]["version"].as_string() == "1.0.0");
    assert(config["application"]["fullscreen"].as_bool() == false);
    assert(config["application"]["resolution"]["width"].as_int64() == 1920);
    assert(config["application"]["resolution"]["height"].as_int64() == 1080);
    std::cout << "  Application config: OK" << '\n';

    assert(config["graphics"]["vsync"].as_bool() == true);
    assert(config["graphics"]["msaa"].as_int64() == 4);
    assert(config["graphics"]["shadow_quality"].as_string() == "high");
    std::cout << "  Graphics config: OK" << '\n';

    assert(config["audio"]["master_volume"].as_double() == 0.8);
    assert(config["audio"]["music_volume"].as_double() == 0.6);
    assert(config["audio"]["sfx_volume"].as_double() == 1.0);
    std::cout << "  Audio config: OK" << '\n';

    auto& controls = config["controls"].as_array();
    assert(controls.size() == 3);
    assert(controls[0]["action"].as_string() == "jump");
    assert(controls[0]["key"].as_string() == "Space");
    assert(controls[2]["action"].as_string() == "inventory");
    std::cout << "  Controls array: OK" << '\n';

    // Serialize and verify round-trip
    auto serialized = serialize_pretty(config);
    auto reparsed   = parse(serialized);
    assert(reparsed["application"]["name"].as_string() == "MyGame");
    std::cout << "  Round-trip: OK" << '\n';

    std::cout << "Complex example test passed!" << '\n' << '\n';
}

auto test_modification() -> void {
    std::cout << "=== Test: Value Modification ===" << '\n';

    // Build object programmatically
    Value obj(Object{});
    obj["name"]   = "Player";
    obj["level"]  = 5;
    obj["health"] = 100.0;
    obj["alive"]  = true;

    assert(obj["name"].as_string() == "Player");
    assert(obj["level"].as_int64() == 5);
    std::cout << "  Object building: OK" << '\n';

    // Modify existing values
    obj["level"] = 6;
    assert(obj["level"].as_int64() == 6);
    std::cout << "  Value modification: OK" << '\n';

    // Build array programmatically
    Value arr(Array{});
    arr.push_back(Value(1));
    arr.push_back(Value(2));
    arr.push_back(Value(3));

    assert(arr.as_array().size() == 3);
    assert(arr[2].as_int64() == 3);
    std::cout << "  Array building: OK" << '\n';

    // Nested structure building
    Value root(Object{});
    root["items"] = Array{};
    root["items"].push_back(Object{});
    root["items"][0]["id"]   = 1;
    root["items"][0]["name"] = "Sword";

    assert(root["items"][0]["name"].as_string() == "Sword");
    std::cout << "  Nested building: OK" << '\n';

    std::cout << "All modification tests passed!" << '\n' << '\n';
}

auto main() -> int {
    std::cout << "========================================" << '\n';
    std::cout << "    gkit::resource::metadata Tests     " << '\n';
    std::cout << "========================================" << '\n' << '\n';

    test_value_construction();
    test_parsing_literals();
    test_parsing_numbers();
    test_parsing_strings();
    test_parsing_arrays();
    test_parsing_objects();
    test_serialization();
    test_accessors();
    test_try_parse();
    test_parse_errors();
    test_complex_example();
    test_modification();

    std::cout << "========================================" << '\n';
    std::cout << "         ALL TESTS PASSED!             " << '\n';
    std::cout << "========================================" << '\n';

    return 0;
}
