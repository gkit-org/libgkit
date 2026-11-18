#include "core/reflect/seralize.hpp"

#include "core/object_pool.hpp"
#include "gkit/core/object_id.hpp"
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
        if (obj_ptr == nullptr) throw std::invalid_argument("expired object id");

        auto& db        = ClassDB::instance();
        auto class_info = db.find(obj_ptr->class_name());
        if (!class_info) throw std::invalid_argument("unregistered object type");

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
    SerdeStruct::SerdeStruct(Value v) { // NOLINT(performance-unnecessary-value-param)
        this->from(std::move(v));
    }

    SerdeStruct::SerdeStruct(const ObjectId v) {
        this->from(v);
    }

    auto SerdeStruct::operator[](const std::string& key) -> Value& {
        if (!this->available_flag) throw std::logic_error("no serialized value");
        for (const auto& node : this->serde_root->get_children()) {
            if (node->get_key() == key) {
                return node->get_value();
            }
        }

        throw std::invalid_argument("element not found");
    }

    auto SerdeStruct::from(Value v) -> void { // NOLINT(performance-unnecessary-value-param)
        this->serde_root.reset();
        this->available_flag = false;
        this->serde_root     = std::make_unique<SerdeNode>("", std::move(v));
        this->available_flag = true;
    }

    auto SerdeStruct::from(ObjectId v) -> void {
        this->serde_root.reset();
        this->available_flag = false;
        if (!v.available()) throw std::invalid_argument("invalid object id");

        this->serde_root = std::make_unique<SerdeNode>("", Type::Object);
        if (!v.with([&](Object& obj) {
                expand_object_children(*this->serde_root, &obj);
            }))
            throw std::invalid_argument("expired object id");
        this->available_flag = true;
    }

    /**
     * SerdeNode
     */
    SerdeNode::SerdeNode(const std::string& k, const Value& v) {
        this->key  = k;
        this->type = v.type();
        switch (this->type) {
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

    SerdeNode::SerdeNode(const std::string& k, Type t) {
        this->key  = k;
        this->type = t;
    }

    auto SerdeNode::add_child(SerdeNode&& child) -> void {
        this->children.emplace_back(std::make_unique<SerdeNode>(std::move(child)));
    }
} // namespace gkit::core::reflect
