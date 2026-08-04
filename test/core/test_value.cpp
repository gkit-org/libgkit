 // NOLINTBEGIN(google-readability-avoid-underscore-in-googletest-name)
#include "gkit/core/object_pool.hpp"
#include "gkit/core/reflect/registry.hpp"
#include "gkit/core/value.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#define TEST(cond, msg)                                                                   \
    do {                                                                                  \
        if (!(cond)) {                                                                    \
            std::cerr << "FAIL: " << msg << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
            return false;                                                                 \
        } else {                                                                          \
            std::cout << "PASS: " << msg << '\n';                                         \
        }                                                                                 \
    } while (0)

using gkit::core::Array;
using gkit::core::Map;
using gkit::core::Null;
using gkit::core::ObjectId;
using gkit::core::Type;
using gkit::core::UniqueObject;
using gkit::core::Value;

struct TestObject : gkit::core::Object {
    int x = 42;
};

struct OtherObject : gkit::core::Object {
    float y = 3.14f;
};

auto test_store_values() -> bool {
    std::cout << "\n=== test_store_values ===\n";

    // Default construction → Null
    Value v_default;
    TEST(v_default.is_null(), "Default construction: is_null");
    TEST(v_default.type() == Type::Null, "Default construction: type is Null");

    // Null
    Value v_null{Null{}};
    TEST(v_null.is_null(), "Null constructor: is_null");
    TEST(!v_null.is_bool(), "Null constructor: !is_bool");
    TEST(!v_null.is_number(), "Null constructor: !is_number");

    // Bool
    Value v_bool{true};
    TEST(v_bool.is_bool(), "Bool constructor: is_bool");
    TEST(v_bool.as_bool(), "Bool constructor: as_bool == true");
    TEST(v_bool.type() == Type::Bool, "Bool constructor: type is Bool");

    Value v_bool_false{false};
    TEST(!v_bool_false.as_bool(), "Bool constructor: as_bool == false");

    // Integer (int64_t)
    Value v_int64{std::int64_t{-42}};
    TEST(v_int64.is_number(), "int64 constructor: is_number");
    TEST(v_int64.is_number_integer(), "int64 constructor: is_number_integer");
    TEST(!v_int64.is_number_float(), "int64 constructor: !is_number_float");
    TEST(v_int64.as_int64() == -42, "int64 constructor: as_int64 == -42");
    TEST(v_int64.type() == Type::Number, "int64 constructor: type is Number");

    // Float
    Value v_float{3.14f};
    TEST(v_float.is_number(), "Float constructor: is_number");
    TEST(v_float.is_number_float(), "Float constructor: is_number_float");
    TEST(!v_float.is_number_integer(), "Float constructor: !is_number_integer");
    TEST(v_float.as_float() == 3.14f, "Float constructor: as_float == 3.14");

    // Int (implicit → int64_t)
    Value v_int{7};
    TEST(v_int.is_number_integer(), "Int constructor: is_number_integer");
    TEST(v_int.as_int64() == 7, "Int constructor: as_int64 == 7");

    // const char* → string
    Value v_cstr{"hello"};
    TEST(v_cstr.is_string(), "const char* constructor: is_string");
    TEST(v_cstr.as_string() == "hello", "const char* constructor: as_string == \"hello\"");
    TEST(v_cstr.type() == Type::String, "const char* constructor: type is String");

    // std::string
    Value v_str{std::string("world")};
    TEST(v_str.is_string(), "std::string constructor: is_string");
    TEST(v_str.as_string() == "world", "std::string constructor: as_string == \"world\"");

    // Array
    Value v_arr{Array{Value{1}, Value{2}, Value{3}}};
    TEST(v_arr.is_array(), "Array constructor: is_array");
    TEST(v_arr.as_array().size() == 3, "Array constructor: size == 3");
    TEST(v_arr.as_array()[0].as_int64() == 1, "Array constructor: [0] == 1");
    TEST(v_arr.as_array()[1].as_int64() == 2, "Array constructor: [1] == 2");
    TEST(v_arr.as_array()[2].as_int64() == 3, "Array constructor: [2] == 3");
    TEST(v_arr.type() == Type::Array, "Array constructor: type is Array");

    // Map
    Value v_map{Map{{"k1", Value{10}}, {"k2", Value{20}}}};
    TEST(v_map.is_map(), "Map constructor: is_map");
    TEST(v_map.as_map().size() == 2, "Map constructor: size == 2");
    TEST(v_map.as_map().at("k1").as_int64() == 10, "Map constructor: [\"k1\"] == 10");
    TEST(v_map.as_map().at("k2").as_int64() == 20, "Map constructor: [\"k2\"] == 20");
    TEST(v_map.type() == Type::Map, "Map constructor: type is Map");

    // ObjectId
    ObjectId test_id = gkit::core::ObjectIdAllocator::instance().new_one();
    Value v_objid{test_id};
    TEST(v_objid.is_object(), "ObjectId constructor: is_object");
    TEST(v_objid.is_object_ref(), "ObjectId constructor: is_object_ref");
    TEST(!v_objid.is_object_owner(), "ObjectId constructor: !is_object_owner");
    TEST(v_objid.type() == Type::ObjectId, "ObjectId constructor: type is ObjectId");

    // as_float on int64-backed Number → converts
    TEST(v_int64.as_float() == -42.0f, "as_float on int64 converts to -42.0f");

    // Mutable array/map access
    v_arr.as_array().push_back(Value{4});
    TEST(v_arr.as_array().size() == 4, "Mutable array: push_back → size 4");
    v_map.as_map()["k3"] = Value{30};
    TEST(v_map.as_map().size() == 3, "Mutable map: insert → size 3");

    return true;
}

auto test_set_values() -> bool {
    std::cout << "\n=== test_set_values ===\n";

    Value v_assign;

    v_assign = Null{};
    TEST(v_assign.is_null(), "Assign Null");

    v_assign = true;
    TEST(v_assign.is_bool() && v_assign.as_bool(), "Assign bool: true");

    v_assign = std::int64_t{100};
    TEST(v_assign.as_int64() == 100, "Assign int64: 100");

    v_assign = 42;
    TEST(v_assign.as_int64() == 42, "Assign int: 42");

    v_assign = 2.5f;
    TEST(v_assign.as_float() == 2.5f, "Assign float: 2.5");

    v_assign = "assigned";
    TEST(v_assign.as_string() == "assigned", "Assign const char*: \"assigned\"");

    v_assign = std::string("moved_str");
    TEST(v_assign.as_string() == "moved_str", "Assign std::string");

    v_assign = Array{Value{5}, Value{6}};
    TEST(v_assign.is_array() && v_assign.as_array().size() == 2, "Assign Array");

    v_assign = Map{{"a", Value{1}}};
    TEST(v_assign.is_map() && v_assign.as_map().size() == 1, "Assign Map");

    // Reassign between different types
    Value v_retype{1};
    v_retype = std::string("switched");
    TEST(v_retype.is_string() && v_retype.as_string() == "switched", "Reassign int → string");
    v_retype = Value{3.0f};
    TEST(v_retype.is_number_float(), "Reassign string → float");

    // Copy assignment
    Value v_src{std::int64_t{42}};
    Value v_cpy;
    v_cpy = v_src;
    TEST(v_cpy.as_int64() == 42, "Copy assignment: value matches");
    v_cpy = std::int64_t{99};
    TEST(v_src.as_int64() == 42, "Deep copy: original unchanged");
    TEST(v_cpy.as_int64() == 99, "Deep copy: modification applied to copy");

    // Self-assignment
    Value v_self{std::int64_t{7}};
    v_self = v_self;
    TEST(v_self.as_int64() == 7, "Self-assignment");

    // Move assignment
    Value v_move_to_dest;
    v_move_to_dest = std::move(v_retype);
    TEST(v_move_to_dest.is_number_float(), "Move assignment: type preserved");
    TEST(v_move_to_dest.as_float() == 3.0f, "Move assignment: value preserved");

    return true;
}

auto test_safe_object_handling() -> bool {
    std::cout << "\n=== test_safe_object_handling ===\n";

    // UniqueObject via string constructor → is_object_owner
    Value v_owner{*UniqueObject::create_with_classname(std::string("TestObject"))};
    TEST(v_owner.is_object(), "UniqueObject: is_object");
    TEST(v_owner.is_object_owner(), "UniqueObject: is_object_owner");
    TEST(!v_owner.is_object_ref(), "UniqueObject: !is_object_ref");

    // as_object → returns ObjectId
    ObjectId owned_id = v_owner.as_object();
    TEST(owned_id.available(), "as_object → valid ObjectId");

    // as_object_ptr → non-null (backed by a real pool object)
    TEST(v_owner.as_object_ptr() != nullptr, "as_object_ptr with registered class → non-null");

    // try_as on UniqueObject backed by real object → valid pointer
    auto* owned_ptr = v_owner.try_as<TestObject>();
    TEST(owned_ptr != nullptr, "try_as on UniqueObject with backing ptr → non-null");

    // Create a real object via ObjectPool for valid pointer tests
    {
        auto pool_result = gkit::core::ObjectPool::instance().create<TestObject>();
        TEST(pool_result.has_value(), "ObjectPool::create<TestObject> succeeds");
        ObjectId valid_id = pool_result->first;

        Value v_ref_obj{valid_id};
        TEST(v_ref_obj.is_object(), "ObjectId with backing object: is_object");
        TEST(v_ref_obj.is_object_ref(), "ObjectId with backing object: is_object_ref");
        TEST(!v_ref_obj.is_object_owner(), "ObjectId with backing object: !is_object_owner");

        auto* obj_ptr = v_ref_obj.as_object_ptr();
        TEST(obj_ptr != nullptr, "as_object_ptr with backing object → non-null");

        auto& test_obj = v_ref_obj.as<TestObject>();
        TEST(test_obj.x == 42, "as<TestObject>() → correct type, x=42");

        const Value& cv_ref_obj = v_ref_obj;
        const auto& c_test_obj  = cv_ref_obj.as<TestObject>();
        TEST(c_test_obj.x == 42, "const as<TestObject>() → correct type");

        auto* try_ptr = v_ref_obj.try_as<TestObject>();
        TEST(try_ptr != nullptr, "try_as<TestObject>() → non-null");
        TEST(try_ptr->x == 42, "try_as<TestObject>() → x=42");

        const auto* c_try_ptr = cv_ref_obj.try_as<TestObject>();
        TEST(c_try_ptr != nullptr, "const try_as<TestObject>() → non-null");

        auto* wrong_try = v_ref_obj.try_as<OtherObject>();
        TEST(wrong_try == nullptr, "try_as<OtherObject>() → nullptr (wrong type)");

        TestObject fallback_obj;
        auto* ptr_or_fallback = v_ref_obj.as_object_or(&fallback_obj);
        TEST(ptr_or_fallback != nullptr, "as_object_or: not null");
        TEST(ptr_or_fallback != &fallback_obj, "as_object_or: returns stored ptr, not fallback");

        gkit::core::ObjectPool::instance().release(valid_id);
    }

    // try_as on non-object → nullptr
    Value v_non_obj{42};
    auto* non_obj_try = v_non_obj.try_as<TestObject>();
    TEST(non_obj_try == nullptr, "try_as on non-object → nullptr");

    // as_object_or on non-object → returns fallback
    TestObject fallback_obj2;
    Value v_str_for_test{"text"};
    auto* or_fallback = v_str_for_test.as_object_or(&fallback_obj2);
    TEST(or_fallback == &fallback_obj2, "as_object_or on non-object → fallback");

    // as_object_ptr on non-object → nullptr
    TEST(v_str_for_test.as_object_ptr() == nullptr, "as_object_ptr on non-object → nullptr");

    // ObjectId round-trip via operator==
    ObjectId test_id = gkit::core::ObjectIdAllocator::instance().new_one();
    Value v_ref{test_id};
    TEST(v_ref.is_object(), "ObjectId Value: is_object");
    TEST(v_ref.is_object_ref(), "ObjectId Value: is_object_ref");
    TEST(!v_ref.is_object_owner(), "ObjectId Value: !is_object_owner");
    TEST(v_ref.as_object() == test_id, "ObjectId Value: operator== round-trip");

    return true;
}

auto test_error_handling() -> bool {
    std::cout << "\n=== test_error_handling ===\n";
    // Note: Unchecked accessors (as_bool, as_int64, as_array, operator[], push_back)
    // are noexcept and call std::terminate() on wrong types.
    // Use is_*() guards or safe accessors (at, as_*_or, try_as, contains).

    // operator[] const on Map missing key → std::out_of_range
    Value v_map_err{Map{{"key", Value{1}}}};
    bool caught_out_of_range = false;
    try {
        const Value& cref = v_map_err;
        (void)cref["missing"];
    } catch (const std::out_of_range&) {
        caught_out_of_range = true;
    }
    TEST(caught_out_of_range, "const operator[] on Map missing key → std::out_of_range");

    // at() on missing key → std::nullopt
    auto missing_key = v_map_err.at("nope");
    TEST(!missing_key.has_value(), "at(\"missing\") → std::nullopt");

    // at() on existing key → has value
    auto existing_key = v_map_err.at("key");
    TEST(existing_key.has_value(), "at(\"key\") → has value");
    TEST(existing_key->get().as_int64() == 1, "at(\"key\") → value == 1");

    // at() with index out-of-bounds → std::nullopt
    Value v_arr_temp{Array{Value{1}, Value{2}}};
    auto out_of_bounds = v_arr_temp.at(5);
    TEST(!out_of_bounds.has_value(), "at(5) on array[0,1] → std::nullopt");

    // at() with valid index → has value
    auto valid_idx = v_arr_temp.at(0);
    TEST(valid_idx.has_value(), "at(0) → has value");
    TEST(valid_idx->get().as_int64() == 1, "at(0) → value == 1");

    // at() on non-array → std::nullopt
    Value v_bool{true};
    auto non_arr_at = v_bool.at(0);
    TEST(!non_arr_at.has_value(), "at(0) on non-array → std::nullopt");

    // at(key) on non-map → std::nullopt
    auto non_map_at = v_bool.at("key");
    TEST(!non_map_at.has_value(), "at(\"key\") on non-map → std::nullopt");

    // contains on non-map → false
    TEST(!v_bool.contains("anything"), "contains on non-map → false");

    // Safe fallback accessors
    Value v_float_num{3.14f};
    Value v_int64{std::int64_t{-42}};
    TEST(v_float_num.as_int64_or(999) == 999, "as_int64_or on float → fallback 999");
    TEST(v_int64.as_int64_or(999) == -42, "as_int64_or on int64 → value -42");
    TEST(v_bool.as_float_or(1.5f) == 1.5f, "as_float_or on bool → fallback 1.5");
    TEST(v_int64.as_bool_or(true), "as_bool_or on int64 → fallback true");
    TEST(!v_int64.as_bool_or(false), "as_bool_or on int64 → fallback false");

    const std::string fallback_str = "default";
    TEST(&v_int64.as_string_or(fallback_str) == &fallback_str, "as_string_or on int64 → fallback");

    // as_float_or on int64-backed Number → returns converted float
    Value v_int_for_or{std::int64_t{42}};
    TEST(v_int_for_or.as_float_or(0.0f) == 42.0f, "as_float_or on int64 → returns 42.0f (converted)");

    // Guard pattern: is_*() check before as_*()
    TEST(v_float_num.is_number_float(), "Guard: is_number_float before as_float");
    TEST(!v_float_num.is_number_integer(), "Guard: !is_number_integer for float value");
    TEST(v_int64.is_number_integer(), "Guard: is_number_integer for int64 value");

    return true;
}

auto test_template_conversion() -> bool {
    std::cout << "\n=== test_template_conversion ===\n";

    // emplace_back on Array (template method)
    Value v_emplace_arr{Array{Value{10}}};
    v_emplace_arr.emplace_back(20);
    v_emplace_arr.emplace_back(std::string("thirty"));
    TEST(v_emplace_arr.as_array().size() == 3, "emplace_back: size == 3");
    TEST(v_emplace_arr.as_array()[1].as_int64() == 20, "emplace_back: [1] == 20");
    TEST(v_emplace_arr.as_array()[2].as_string() == "thirty", "emplace_back: [2] == \"thirty\"");

    // push_back on Array
    v_emplace_arr.push_back(Value{3.14f});
    TEST(v_emplace_arr.as_array().size() == 4, "push_back → size 4");
    TEST(v_emplace_arr.as_array()[3].is_number_float(), "push_back: [3] is float");

    // as<T>() with wrong dynamic type → invalid_argument
    {
        auto pool_result = gkit::core::ObjectPool::instance().create<TestObject>();
        TEST(pool_result.has_value(), "ObjectPool::create<TestObject> for template conversion test");
        Value v_obj{pool_result->first};

        bool caught_invalid_arg = false;
        try {
            (void)v_obj.as<OtherObject>();
        } catch (const std::invalid_argument&) {
            caught_invalid_arg = true;
        }
        TEST(caught_invalid_arg, "as<OtherObject>() on TestObject → std::invalid_argument");

        bool caught_const_invalid = false;
        try {
            const Value& cv = v_obj;
            (void)cv.as<OtherObject>();
        } catch (const std::invalid_argument&) {
            caught_const_invalid = true;
        }
        TEST(caught_const_invalid, "const as<OtherObject>() on TestObject → std::invalid_argument");

        const Value& cv2    = v_obj;
        const auto* c_wrong = cv2.try_as<OtherObject>();
        TEST(c_wrong == nullptr, "const try_as<OtherObject>() → nullptr");

        gkit::core::ObjectPool::instance().release(pool_result->first);
    }

    return true;
}

auto test_lifetime_safety() -> bool {
    std::cout << "\n=== test_lifetime_safety ===\n";

    // Copy Value with UniqueObject → logic_error
    {
        UniqueObject uo{*UniqueObject::create_with_classname(std::string("CopyTest"))};
        Value vo(std::move(uo));
        bool caught_logic_copy = false;
        try {
            Value copied = vo; // NOLINT(performance-unnecessary-copy-initialization)
            (void)copied;
        } catch (const std::logic_error&) {
            caught_logic_copy = true;
        }
        TEST(caught_logic_copy, "Copy Value with UniqueObject → std::logic_error");
    }

    // Copy assign Value with UniqueObject → logic_error
    {
        UniqueObject uo{*UniqueObject::create_with_classname(std::string("CopyAssignTest"))};
        Value vo(std::move(uo));
        Value dest;
        bool caught_logic_assign = false;
        try {
            dest = vo;
        } catch (const std::logic_error&) {
            caught_logic_assign = true;
        }
        TEST(caught_logic_assign, "Copy-assign Value with UniqueObject → std::logic_error");
    }

    // Move Value with UniqueObject → succeeds
    {
        UniqueObject uo{*UniqueObject::create_with_classname(std::string("MoveTest"))};
        Value vo(std::move(uo));
        TEST(vo.is_object_owner(), "Move: source is_object_owner");

        Value moved = std::move(vo);
        TEST(moved.is_object_owner(), "Move: dest is_object_owner");
        TEST(vo.is_object_owner(), "Move: source still is_object_owner (moved-from)");
        TEST(vo.as_object_ptr() == nullptr, "Move: source obj_ptr is nullptr");
    }

    // Deep copy: non-UniqueObject copy is independent
    Value original{Array{Value{1}, Value{2}}};
    Value copied = original;
    copied.as_array().push_back(Value{3});
    TEST(original.as_array().size() == 2, "Deep copy: original size unchanged");
    TEST(copied.as_array().size() == 3, "Deep copy: copy size increased");

    // Copy assignment safety: original non-UniqueObject stays valid
    Value src{std::int64_t{77}};
    Value dst;
    dst = src;
    TEST(dst.as_int64() == 77, "Copy assign: value matches");
    dst = std::string("changed");
    TEST(src.as_int64() == 77, "Copy assign: original unchanged");
    TEST(dst.as_string() == "changed", "Copy assign: dst modified");

    // Map non-const operator[] inserts default on missing key
    Value v_map_insert{Map{}};
    v_map_insert["new_key"] = Value{99};
    TEST(v_map_insert.as_map().size() == 1, "Map operator[]: size == 1 after insert");
    TEST(v_map_insert["new_key"].as_int64() == 99, "Map operator[]: inserted value == 99");

    return true;
} 

auto main() -> int {
    auto& db = gkit::core::reflect::ClassDB::instance();
    db.regist<TestObject>("TestObject");
    db.regist<TestObject>("CopyTest");
    db.regist<TestObject>("CopyAssignTest");
    db.regist<TestObject>("MoveTest");

    bool all_passed = true;

    all_passed &= test_store_values();
    all_passed &= test_set_values();
    all_passed &= test_safe_object_handling();
    all_passed &= test_error_handling();
    all_passed &= test_template_conversion();
    all_passed &= test_lifetime_safety();

    if (all_passed) {
        std::cout << "\nAll tests passed!\n";
        return 0;
    }

    std::cerr << "\nSome tests failed.\n"; 
    return 1;
}
// NOLINTEND(google-readability-avoid-underscore-in-googletest-name)
