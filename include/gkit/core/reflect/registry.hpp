#pragma once

#include "gkit/core/object.hpp"
#include "gkit/core/templates/singleton.hpp"
#include "gkit/core/unique_object.hpp"
#include "gkit/core/value.hpp"

#include <concepts>
#include <functional>
#include <map>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace gkit::core::reflect {
    /** @brief A type with a public static registration method taking ClassDB& and returning void.
     * Each reflected class defines its own method, including derived classes that inherit one. */
    template<class T>
    concept Resistible = requires(ClassDB& db) {
        static_cast<void (*)(ClassDB&)>(&T::_regist_method);
        T::_regist_method(db);
    };

    /** @brief Invoke type registration methods left to right.
     * @tparam Types Reflected types, with every parent registered before its children.
     * @param db The registry being populated. Methods must use this instance during registry construction.
     * @note A registration exception propagates to the caller and skips the rest of the list. */
    template<Resistible... Types>
    auto regist_types(ClassDB& db) -> void;

    struct FieldDesc final {
        std::string name;
        Type value_type;
    };

    class ClassInfo final {
    public:
        using Getter  = std::function<Value(const Object*)>;
        using Setter  = std::function<void(Object*, const Value&)>;
        using Factory = std::function<UniqueObject()>;

        Factory factory;
        std::string class_name;
        std::string parent_class_name;
        std::vector<FieldDesc> fields;

        [[nodiscard]] auto parent() const -> const ClassInfo*;

        [[nodiscard]] auto get_field(const Object* obj, const std::string& name) const -> std::optional<Value>;
        auto set_field(Object* obj, const std::string& name, const Value& v) const -> bool;

        template<typename Fn>
        auto for_each_field(Fn&& fn) const -> void;

    private:
        friend class ClassDB;

        std::shared_ptr<const ClassInfo> parent_info;
        std::type_index cpp_type{typeid(void)};

        struct FieldAccess {
            Getter getter;
            Setter setter;
        };

        std::vector<FieldAccess> accessors;
    };

    class ClassDB final : public gkit::core::templates::Singleton<ClassDB> {
        friend gkit::core::templates::Singleton<ClassDB>;

    public:
        /** @brief Access the registry with all built-in types registered, including during static initialization. */
        static auto instance() -> ClassDB&;

        /** @brief Register a type after its parent. Each chained field or property edit publishes its own snapshot. */
        template<IsObject T>
        auto regist(const std::string& class_name, const std::string& parent = "") -> ClassDB&;

        template<IsObject T, IsValueType F>
        auto add_field(const std::string& class_name, const std::string& field_name, F T::* member) -> ClassDB&;

        /** @brief Bind a const getter and an optional setter for the same native value type.
         * Omit the setter for a read-only property. Getters may return a value or a const reference. */
        template<IsObject T, typename R, typename A = std::remove_cvref_t<R>>
            requires std::same_as<std::remove_cvref_t<R>, std::remove_cvref_t<A>>
        auto add_property(const std::string& class_name,
                          const std::string& field_name,
                          R (T::*getter)() const,
                          void (T::*setter)(A) = nullptr) -> ClassDB&;

        /** @brief Bind accessors that already exchange reflected Value objects. */
        template<IsObject T>
        auto add_property(const std::string& class_name,
                          const std::string& field_name,
                          std::function<Value(const T&)> getter,
                          std::function<void(T&, const Value&)> setter = nullptr) -> ClassDB&;

        [[nodiscard]] auto find(const std::string& class_name) const -> std::shared_ptr<const ClassInfo>;
        [[nodiscard]] auto find_with_raw(const char* raw_name) const -> std::shared_ptr<const ClassInfo>;
        [[nodiscard]] auto create(const std::string& class_name) const -> std::optional<UniqueObject>;

    private:
        ClassDB();

        mutable std::shared_mutex registry_mutex;
        std::map<std::string, std::shared_ptr<const ClassInfo>> classes;
        std::unordered_map<std::string, std::string> raw2_class_name;
        std::unordered_map<std::type_index, std::string> type_names;

        template<IsObject T, typename F, typename Getter, typename Setter>
        auto add_accessors(const std::string& class_name, FieldDesc field, Getter getter, Setter setter) -> ClassDB&;

        auto publish(std::shared_ptr<ClassInfo> info) -> void;
        auto editable(const std::string& name, std::type_index type) -> std::shared_ptr<ClassInfo>;
        static auto check_field(const ClassInfo& info, const std::string& name) -> void;
    };

    // =========================================================================
    // Template implementation — detail helpers
    // =========================================================================

    namespace detail {
        template<typename F>
        constexpr auto deduce_type() -> Type {
            if constexpr (std::is_same_v<F, bool>) {
                return Type::Bool;
            } else if constexpr (std::is_integral_v<F> || std::is_floating_point_v<F>) {
                return Type::Number;
            } else if constexpr (std::is_same_v<F, std::string>) {
                return Type::String;
            } else if constexpr (std::is_same_v<F, gkit::core::Array>) {
                return Type::Array;
            } else if constexpr (std::is_same_v<F, gkit::core::Map>) {
                return Type::Map;
            } else {
                return Type::Null;
            }
        }

        template<typename T, typename Obj>
        auto field_owner(Obj* object) -> T& {
            auto* owner = dynamic_cast<T*>(object);
            if (!owner) throw std::invalid_argument("wrong reflected object type");
            return *owner;
        }

        template<typename T, typename Getter>
        auto read_field(const Getter& getter, const Object* object) -> Value {
            decltype(auto) value = std::invoke(getter, field_owner<const T>(object));
            using F              = std::remove_cvref_t<decltype(value)>;
            if constexpr (std::integral<F> && !std::same_as<F, bool>) {
                return Value(static_cast<std::int64_t>(value));
            } else if constexpr (std::floating_point<F>) {
                return Value(static_cast<float>(value));
            } else if constexpr (std::same_as<F, Value> || deduce_type<F>() != Type::Null) {
                return Value(std::forward<decltype(value)>(value));
            } else {
                return {};
            }
        }

        template<typename F>
        auto unpack_field(const Value& value) -> decltype(auto) {
            if constexpr (std::same_as<F, Value>) {
                return (value);
            } else if constexpr (std::same_as<F, bool>) {
                return value.as_bool();
            } else if constexpr (std::integral<F>) {
                const auto number = value.as_int64();
                using Integer = std::conditional_t<std::is_signed_v<F>, std::make_signed_t<F>, std::make_unsigned_t<F>>;
                if (!std::in_range<Integer>(number)) throw std::out_of_range("integer field overflow");
                return static_cast<F>(number);
            } else if constexpr (std::floating_point<F>) {
                return static_cast<F>(value.as_float());
            } else if constexpr (std::same_as<F, String>) {
                return value.as_string();
            } else if constexpr (std::same_as<F, Array>) {
                return value.as_array();
            } else if constexpr (std::same_as<F, Map>) {
                return value.as_map();
            }
        }

        template<typename T, typename F, typename Setter>
        auto write_field(const Setter& setter, Object* object, const Value& value) -> void {
            auto& owner = field_owner<T>(object);
            if constexpr (std::same_as<F, Value> || deduce_type<F>() != Type::Null) {
                if constexpr (std::is_member_object_pointer_v<Setter>) {
                    std::invoke(setter, owner) = unpack_field<F>(value);
                } else {
                    std::invoke(setter, owner, unpack_field<F>(value));
                }
            }
        }
    } // namespace detail

    // =========================================================================
    // Template implementation — ClassDB / ClassInfo
    // =========================================================================

    template<Resistible... Types>
    auto regist_types(ClassDB& db) -> void {
        (Types::_regist_method(db), ...);
    }

    template<IsObject T>
    auto ClassDB::regist(const std::string& class_name, const std::string& parent) -> ClassDB& {
        std::unique_lock guard(this->registry_mutex);
        if (class_name.empty() || this->classes.contains(class_name) || this->type_names.contains(typeid(T))) {
            throw std::invalid_argument("empty or duplicate class registration");
        }
        auto info               = std::make_shared<ClassInfo>();
        info->class_name        = class_name;
        info->parent_class_name = parent;
        info->cpp_type          = typeid(T);
        if (!parent.empty()) {
            auto it = this->classes.find(parent);
            if (it == this->classes.end()) {
                throw std::invalid_argument("register parent '" + parent + "' before child '" + class_name + "'");
            }
            info->parent_info = it->second;
        }
        if constexpr (std::is_default_constructible_v<T>) {
            info->factory = []() -> UniqueObject {
                return UniqueObject::create<T>();
            };
        }
        this->publish(std::move(info));
        return *this;
    }

    template<IsObject T, IsValueType F>
    auto ClassDB::add_field(const std::string& class_name, const std::string& field_name, F T::* member) -> ClassDB& {
        return add_accessors<T, F>(class_name, {field_name, detail::deduce_type<F>()}, member, member);
    }

    template<IsObject T, typename R, typename A>
        requires std::same_as<std::remove_cvref_t<R>, std::remove_cvref_t<A>>
    auto ClassDB::add_property(const std::string& class_name,
                               const std::string& field_name,
                               R (T::*getter)() const,
                               void (T::*setter)(A)) -> ClassDB& {
        using F = std::remove_cvref_t<R>;
        static_assert(std::same_as<F, Value> || detail::deduce_type<F>() != Type::Null, "unsupported property type");
        return add_accessors<T, F>(class_name, {field_name, Type::Null}, getter, setter);
    }

    template<IsObject T>
    auto ClassDB::add_property(const std::string& class_name,
                               const std::string& field_name,
                               std::function<Value(const T&)> getter,
                               std::function<void(T&, const Value&)> setter) -> ClassDB& {
        return add_accessors<T, Value>(class_name, {field_name, Type::Null}, std::move(getter), std::move(setter));
    }

    template<IsObject T, typename F, typename Getter, typename Setter>
    auto ClassDB::add_accessors(const std::string& class_name, FieldDesc field, Getter getter, Setter setter)
        -> ClassDB& {
        if (!getter) throw std::invalid_argument("empty field getter");
        ClassInfo::FieldAccess access;
        access.getter = std::bind_front(detail::read_field<T, Getter>, std::move(getter));
        if (setter) {
            access.setter = std::bind_front(detail::write_field<T, F, Setter>, std::move(setter));
        }
        std::unique_lock guard(this->registry_mutex);
        auto info = this->editable(class_name, typeid(T));
        check_field(*info, field.name);
        info->fields.push_back(std::move(field));
        info->accessors.push_back(std::move(access));
        this->publish(std::move(info));
        return *this;
    }

    template<typename Fn>
    auto ClassInfo::for_each_field(Fn&& fn) const -> void {
        if (this->parent_info != nullptr) {
            this->parent_info->for_each_field(std::forward<Fn>(fn));
        }
        for (size_t i = 0; i < this->fields.size(); ++i) {
            const Setter* setter_ptr = this->accessors[i].setter ? &this->accessors[i].setter : nullptr;
            fn(this->fields[i], this->accessors[i].getter, setter_ptr);
        }
    }

} // namespace gkit::core::reflect
