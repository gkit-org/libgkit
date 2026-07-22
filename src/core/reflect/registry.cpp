#include "gkit/core/reflect/registry.hpp"

#include <utility>

namespace gkit::core {

    auto ClassDB::instance() -> ClassDB& {
        static ClassDB db;
        return db;
    }

    auto ClassDB::find(const std::string& class_name) const -> const ClassInfo* {
        auto it = this->classes.find(class_name);
        if (it != this->classes.end()) {
            return &it->second;
        }
        return nullptr;
    }

    auto ClassInfo::parent() const -> const ClassInfo* {
        if (this->parent_cache == nullptr && !this->parent_class_name.empty()) {
            auto* found        = const_cast<ClassInfo*>(ClassDB::instance().find(this->parent_class_name));
            this->parent_cache = found;
        }
        return this->parent_cache;
    }

    auto ClassInfo::get_field(const void* obj, const std::string& name) const -> std::optional<Value> {
        for (size_t i = 0; i < this->fields.size(); ++i) {
            if (this->fields[i].name == name) {
                return this->getters[i](obj);
            }
        }
        if (const auto* p = this->parent()) {
            return p->get_field(obj, name);
        }
        return std::nullopt;
    }

    auto ClassInfo::set_field(void* obj, const std::string& name, const Value& v) const -> bool {
        for (size_t i = 0; i < this->fields.size(); ++i) {
            if (this->fields[i].name == name) {
                if (this->setters[i]) {
                    this->setters[i](obj, v);
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

} // namespace gkit::core
