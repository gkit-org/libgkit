#pragma once

#include "gkit/core/scene/object.hpp"
#include "gkit/core/scene/singleton.hpp"
#include "gkit/core/value.hpp"

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <type_traits>
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
        using Getter = std::function<Value(const void*)>;
        using Setter = std::function<void(void*, const Value&)>;

        std::string class_name;
        std::string parent_class_name;
        std::vector<FieldDesc> fields;

        [[nodiscard]] auto parent() const -> const ClassInfo*;

        [[nodiscard]] auto get_field(const void* obj, const std::string& name) const -> std::optional<Value>;
        auto set_field(void* obj, const std::string& name, const Value& v) const -> bool;

        template<typename Fn>
        auto for_each_field(Fn&& fn) const -> void;

    private:
        friend class ClassDB;
        mutable ClassInfo* parent_cache = nullptr;

        std::vector<Getter> getters;
        std::vector<Setter> setters;
    };

    class ClassDB final : public gkit::core::scene::Singleton<ClassDB> {
        friend gkit::core::scene::Singleton<ClassDB>;

    public:
        template<scene::IsObject T>
        auto regist(const std::string& class_name, const std::string& parent = "") -> ClassDB&;

        template<scene::IsObject T, IsValueType F>
        auto add_field(const std::string& class_name, const std::string& field_name, F T::* member) -> ClassDB&;

        template<scene::IsObject T>
        auto add_property(const std::string& class_name,
                          const std::string& field_name,
                          std::function<Value(const T&)> getter,
                          std::function<void(T&, const Value&)> setter = nullptr) -> ClassDB&;

        [[nodiscard]] auto find(const std::string& class_name) const -> const ClassInfo*;

    private:
        ClassDB() = default;
        std::map<std::string, ClassInfo> classes;
    };

    // =========================================================================
    // Template implementation — detail helpers
    // =========================================================================

    namespace detail {
        template<typename T, typename F>
        auto make_getter(F T::* member) -> ClassInfo::Getter {
            return [member](const void* obj) -> Value {
                const auto& val = (*static_cast<const T*>(obj)).*member;
                if constexpr (std::is_same_v<F, bool>) {
                    return Value(val);
                } else if constexpr (std::is_same_v<F, std::string>) {
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
            return [member](void* obj, const Value& v) {
                auto& val = (*static_cast<T*>(obj)).*member;
                if constexpr (std::is_same_v<F, bool>) {
                    val = v.as_bool();
                } else if constexpr (std::is_same_v<F, std::string>) {
                    val = v.as_string();
                } else if constexpr (std::is_integral_v<F>) {
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
            } else {
                return Type::Null;
            }
        }
    } // namespace detail

    // =========================================================================
    // Template implementation — ClassDB / ClassInfo
    // =========================================================================

    template<scene::IsObject T>
    auto ClassDB::regist(const std::string& class_name, const std::string& parent) -> ClassDB& {
        auto& info             = this->classes[class_name];
        info.class_name        = class_name;
        info.parent_class_name = parent;
        return *this;
    }

    template<scene::IsObject T, IsValueType F>
    auto ClassDB::add_field(const std::string& class_name, const std::string& field_name, F T::* member) -> ClassDB& {
        auto& info = this->classes.at(class_name);
        info.fields.push_back(FieldDesc{field_name, detail::deduce_type<F>()});
        info.getters.push_back(detail::make_getter(member));
        info.setters.push_back(detail::make_setter(member));
        return *this;
    }

    template<scene::IsObject T>
    auto ClassDB::add_property(const std::string& class_name,
                               const std::string& field_name,
                               std::function<Value(const T&)> getter,
                               std::function<void(T&, const Value&)> setter) -> ClassDB& {
        auto& info = this->classes.at(class_name);
        info.fields.push_back(FieldDesc{field_name, Type::Null});
        info.getters.push_back([getter = std::move(getter)](const void* obj) -> Value {
            return getter(*static_cast<const T*>(obj));
        });
        if (setter != nullptr) {
            info.setters.push_back([setter = std::move(setter)](void* obj, const Value& v) {
                setter(*static_cast<T*>(obj), v);
            });
        } else {
            info.setters.emplace_back();
        }
        return *this;
    }

    template<typename Fn>
    auto ClassInfo::for_each_field(Fn&& fn) const -> void {
        if (this->parent_cache != nullptr) {
            this->parent_cache->for_each_field(std::forward<Fn>(fn));
        }
        for (size_t i = 0; i < this->fields.size(); ++i) {
            const Setter* setter_ptr = this->setters[i] ? &this->setters[i] : nullptr;
            fn(this->fields[i], this->getters[i], setter_ptr);
        }
    }

} // namespace gkit::core::reflect
