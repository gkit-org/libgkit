#include "core/reflect/seralize.hpp"

#include "gkit/core/object_id.hpp"
#include "gkit/core/object_pool.hpp"
#include "gkit/core/reflect/registry.hpp"
#include "gkit/core/value.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace gkit::core::reflect {
    /**
     * @brief Expand an object's registered fields into @p node's children.
     * ObjectId-typed field values do not participate in (de)serialization for now.
     */
    static void expand_object_children(SerdeNode& node, const gkit::core::Object* obj_ptr) {
        if (obj_ptr == nullptr) return;

        auto& db               = ClassDB::instance();
        const auto* class_info = db.find(obj_ptr->class_name());
        if (class_info == nullptr) return;

        class_info->for_each_field(
            [&node, obj_ptr, class_info](const FieldDesc& field, const auto&, const auto*) -> void {
                auto val_opt = class_info->get_field(obj_ptr, field.name);
                if (!val_opt.has_value()) return;
                if (val_opt.value().type() == Type::ObjectId) return;

                node.add_child(SerdeNode(field.name, val_opt.value()));
            });
    }

    /**
     * SerdeStruct
     */
    SerdeStruct::SerdeStruct(Value v) noexcept { // NOLINT(performance-unnecessary-value-param)
        this->from(std::move(v));
    }

    SerdeStruct::SerdeStruct(const ObjectId v) noexcept {
        this->from(v);
    }

    auto SerdeStruct::operator[](const std::string& key) -> Value& {
        for (const auto& node : this->serde_root->get_children()) {
            if (node->get_key() == key) {
                return node->get_value();
            }
        }

        throw std::invalid_argument("element not found");
    }

    auto SerdeStruct::from(Value v) noexcept -> void { // NOLINT(performance-unnecessary-value-param)
        this->serde_root     = std::make_unique<SerdeNode>("", std::move(v));
        this->available_flag = true;
    }

    auto SerdeStruct::from(ObjectId v) noexcept -> void {
        if (!v.available()) return;

        auto* obj_ptr = ObjectPool::instance().deref_from(v);
        if (obj_ptr == nullptr) return;

        this->serde_root = std::make_unique<SerdeNode>("", Type::Object);
        expand_object_children(*this->serde_root, obj_ptr);
        this->available_flag = true;
    }

    /**
     * SerdeNode
     */
    SerdeNode::SerdeNode(const std::string& k, const Value& v) noexcept {
        this->key  = k;
        this->type = v.type();
        switch (this->type) {
        case Type::Object: {
            const auto* obj = v.as_object_ptr();
            if (obj == nullptr) return;

            expand_object_children(*this, obj);
            break;
        }

        case Type::ObjectId: {
            // ObjectId values do not participate in (de)serialization for now.
            return;
        }

        case Type::Array: {
            const auto& arr = v.as_array();
            for (const auto& item : arr) {
                if (item.type() == Type::ObjectId) continue;
                this->add_child(SerdeNode("", item));
            }
            break;
        }

        case Type::Map: {
            const auto& map = v.as_map();
            for (const auto& p : map) {
                if (p.second.type() == Type::ObjectId) continue;
                this->add_child(SerdeNode(p.first, p.second));
            }
            break;
        }

        default: {
            // Basic data type: keep the payload in value.
            this->value = v;
            break;
        }
        }
    }

    SerdeNode::SerdeNode(const std::string& k, Type t) noexcept {
        this->key  = k;
        this->type = t;
    }

    auto SerdeNode::add_child(SerdeNode&& child) -> void {
        this->children.emplace_back(std::make_unique<SerdeNode>(std::move(child)));
    }
} // namespace gkit::core::reflect
