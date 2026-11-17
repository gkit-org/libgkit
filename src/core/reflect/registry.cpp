#include "gkit/core/reflect/registry.hpp"

#include "gkit/core/unique_object.hpp"

#include <optional>
#include <utility>

namespace gkit::core::reflect {

    auto ClassDB::publish(std::shared_ptr<ClassInfo> info) -> void {
        auto classes               = this->classes;
        auto names                 = this->type_names;
        auto raw                   = this->raw2_class_name;
        classes[info->class_name]  = info;
        names[info->cpp_type]      = info->class_name;
        raw[info->cpp_type.name()] = info->class_name;
        this->history.push_back(info);
        this->classes.swap(classes);
        this->type_names.swap(names);
        this->raw2_class_name.swap(raw);
    }

    auto ClassDB::editable(const std::string& name, std::type_index type) -> std::shared_ptr<ClassInfo> {
        auto original = this->classes.at(name);
        if (original->cpp_type != type) throw std::invalid_argument("field owner does not match registered type");
        return std::make_shared<ClassInfo>(*original);
    }

    auto ClassDB::check_field(const ClassInfo& info, const std::string& name) -> void {
        if (name.empty()) throw std::invalid_argument("empty field name");
        info.for_each_field([&](const FieldDesc& field, const auto&, const auto*) {
            if (field.name == name) throw std::invalid_argument("duplicate field");
        });
    }

    auto ClassDB::snapshot(const std::string& name) const -> std::shared_ptr<const ClassInfo> {
        std::shared_lock guard(this->registry_mutex);
        auto it = this->classes.find(name);
        return it == this->classes.end() ? nullptr : it->second;
    }

    auto ClassDB::find(const std::string& name) const -> const ClassInfo* {
        return this->snapshot(name).get();
    }

    auto ClassDB::find_with_raw(const char* name) const -> const ClassInfo* {
        if (!name) return nullptr;
        std::shared_lock guard(this->registry_mutex);
        auto it = this->raw2_class_name.find(name);
        return it == this->raw2_class_name.end() ? nullptr : this->classes.at(it->second).get();
    }

    auto ClassDB::create(const std::string& name) const -> std::optional<UniqueObject> {
        auto info = this->snapshot(name);
        if (!info || !info->factory) return std::nullopt;
        auto object = info->factory();
        if (!object.get()) return std::nullopt;
        return object;
    }

    auto ClassInfo::parent() const -> const ClassInfo* {
        return this->parent_info.get();
    }

    auto ClassInfo::get_field(const Object* obj, const std::string& name) const -> std::optional<Value> {
        for (size_t i = 0; i < this->fields.size(); ++i) {
            if (this->fields[i].name == name) {
                return this->accessors[i].getter(obj);
            }
        }
        if (const auto* p = this->parent()) {
            return p->get_field(obj, name);
        }
        return std::nullopt;
    }

    auto ClassInfo::set_field(Object* obj, const std::string& name, const Value& v) const -> bool {
        for (size_t i = 0; i < this->fields.size(); ++i) {
            if (this->fields[i].name == name) {
                if (this->accessors[i].setter) {
                    this->accessors[i].setter(obj, v);
                    return true;
                }
                return false;
            }
        }
        if (const auto* p = this->parent()) {
            return p->set_field(obj, name, v);
        }
        return false;
    }

} // namespace gkit::core::reflect
