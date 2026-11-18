#pragma once

#include "gkit/core/object.hpp"
#include "gkit/core/templates/singleton.hpp"
#include "gkit/core/unique_object.hpp"
#include "gkit/core/value.hpp"

#include <functional>
#include <limits>
#include <map>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace gkit::core::reflect {
    struct RegistHolder {
        explicit RegistHolder(std::function<void()>&& regist_func) { regist_func(); };
    };

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
        template<IsObject T>
        auto regist(const std::string& class_name, const std::string& parent = "") -> ClassDB&;

        template<IsObject T, IsValueType F>
        auto add_field(const std::string& class_name, const std::string& field_name, F T::* member) -> ClassDB&;

        template<IsObject T>
        auto add_property(const std::string& class_name,
                          const std::string& field_name,
                          std::function<Value(const T&)> getter,
                          std::function<void(T&, const Value&)> setter = nullptr) -> ClassDB&;

        [[nodiscard]] auto find(const std::string& class_name) const -> std::shared_ptr<const ClassInfo>;
        [[nodiscard]] auto find_with_raw(const char* raw_name) const -> std::shared_ptr<const ClassInfo>;
        [[nodiscard]] auto create(const std::string& class_name) const -> std::optional<UniqueObject>;

        /** @brief Build a complete description privately, then publish it atomically. */
        template<IsObject T, class Fn>
        auto register_class(const std::string& name, Fn&& configure, const std::string& parent = "") -> void {
            ClassDB staging;
            {
                std::shared_lock guard(this->registry_mutex);
                staging.classes    = this->classes;
                staging.type_names = this->type_names;
            }
            staging.regist<T>(name, parent);
            configure(staging);
            auto completed = std::make_shared<ClassInfo>(*staging.find(name));
            std::unique_lock guard(this->registry_mutex);
            if (this->classes.contains(name) || this->type_names.contains(typeid(T))) {
                throw std::invalid_argument("class already registered");
            }
            this->publish(std::move(completed));
        }

    private:
        ClassDB() = default;
        mutable std::shared_mutex registry_mutex;
        std::map<std::string, std::shared_ptr<const ClassInfo>> classes;
        std::unordered_map<std::string, std::string> raw2_class_name;
        std::unordered_map<std::type_index, std::string> type_names;
        auto publish(std::shared_ptr<ClassInfo> info) -> void;
        auto editable(const std::string& name, std::type_index type) -> std::shared_ptr<ClassInfo>;
        static auto check_field(const ClassInfo& info, const std::string& name) -> void;
    };

    // =========================================================================
    // Template implementation — detail helpers
    // =========================================================================

    namespace detail {
        template<typename T, typename F>
        auto make_getter(F T::* member) -> ClassInfo::Getter {
            return [member](const Object* obj) -> Value {
                const auto* typed = dynamic_cast<const T*>(obj);
                if (!typed) throw std::invalid_argument("wrong reflected object type");
                const auto& val = (*typed).*member;
                if constexpr (std::is_same_v<F, bool>) {
                    return Value(val);
                } else if constexpr (std::is_same_v<F, std::string>) {
                    return Value(val);
                } else if constexpr (std::is_same_v<F, gkit::core::Array>) {
                    return Value(val);
                } else if constexpr (std::is_same_v<F, gkit::core::Map>) {
                    return Value(val);
                } else if constexpr (std::is_integral_v<F>) {
                    return Value(static_cast<std::int64_t>(val));
                } else if constexpr (std::is_floating_point_v<F>) {
                    return Value(static_cast<float>(val));
                }
                return Value(); // noreachable
            };
        }

        template<typename T, typename F>
        auto make_setter(F T::* member) -> ClassInfo::Setter {
            return [member](Object* obj, const Value& v) {
                auto* typed = dynamic_cast<T*>(obj);
                if (!typed) throw std::invalid_argument("wrong reflected object type");
                auto& val = (*typed).*member;
                if constexpr (std::is_same_v<F, bool>) {
                    val = v.as_bool();
                } else if constexpr (std::is_same_v<F, std::string>) {
                    val = v.as_string();
                } else if constexpr (std::is_same_v<F, gkit::core::Array>) {
                    val = v.as_array();
                } else if constexpr (std::is_same_v<F, gkit::core::Map>) {
                    val = v.as_map();
                } else if constexpr (std::is_integral_v<F>) {
                    if (!std::in_range<F>(v.as_int64())) throw std::out_of_range("integer field overflow");
                    val = static_cast<F>(v.as_int64());
                } else if constexpr (std::is_floating_point_v<F>) {
                    val = static_cast<F>(v.as_float());
                }
            };
        }

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
    } // namespace detail

    // =========================================================================
    // Template implementation — ClassDB / ClassInfo
    // =========================================================================

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
            if (it == this->classes.end()) throw std::invalid_argument("register parent before child");
            info->parent_info = it->second;
        }
        if constexpr (std::is_default_constructible_v<T>) {
            info->factory = [] {
                return UniqueObject::create<T>();
            };
        }
        this->publish(std::move(info));
        return *this;
    }

    template<IsObject T, IsValueType F>
    auto ClassDB::add_field(const std::string& class_name, const std::string& field_name, F T::* member) -> ClassDB& {
        std::unique_lock guard(this->registry_mutex);
        auto info = this->editable(class_name, typeid(T));
        check_field(*info, field_name);
        info->fields.push_back(FieldDesc{field_name, detail::deduce_type<F>()});
        info->accessors.push_back({detail::make_getter(member), detail::make_setter(member)});
        this->publish(std::move(info));
        return *this;
    }

    template<IsObject T>
    auto ClassDB::add_property(const std::string& class_name,
                               const std::string& field_name,
                               std::function<Value(const T&)> getter,
                               std::function<void(T&, const Value&)> setter) -> ClassDB& {
        std::unique_lock guard(this->registry_mutex);
        auto info = this->editable(class_name, typeid(T));
        check_field(*info, field_name);
        info->fields.push_back(FieldDesc{field_name, Type::Null});
        if (!getter) throw std::invalid_argument("empty property getter");
        ClassInfo::FieldAccess access;
        access.getter = [getter = std::move(getter)](const Object* obj) -> Value {
            auto* typed = dynamic_cast<const T*>(obj);
            if (!typed) throw std::invalid_argument("wrong reflected object type");
            return getter(*typed);
        };
        if (setter != nullptr) {
            access.setter = [setter = std::move(setter)](Object* obj, const Value& v) {
                auto* typed = dynamic_cast<T*>(obj);
                if (!typed) throw std::invalid_argument("wrong reflected object type");
                setter(*typed, v);
            };
        }
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
