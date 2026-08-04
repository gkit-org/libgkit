#include "gkit/core/unique_object.hpp"

#include "gkit/core/object_id.hpp"
#include "gkit/core/object_pool.hpp"
#include "gkit/core/reflect/registry.hpp"

#include <string>
#include <utility>

namespace gkit::core {
    auto UniqueObject::create_with_classname(const std::string& class_name) noexcept -> std::optional<UniqueObject> {
        return reflect::ClassDB::instance().create(class_name);
    }

    auto UniqueObject::get_id() const noexcept -> const ObjectId& {
        static const ObjectId invalid_id{};
        return this->obj == nullptr ? invalid_id : this->obj->get_object_id();
    }

    UniqueObject::UniqueObject(UniqueObject&& other) noexcept {
        this->obj = other.obj;
        other.obj = nullptr;
    }

    UniqueObject::~UniqueObject() noexcept {
        if (this->obj == nullptr) return;
        auto& obj_pool = ObjectPool::instance();
        obj_pool.release(this->obj->get_object_id());
    }

    auto UniqueObject::operator=(UniqueObject&& other) noexcept -> UniqueObject& {
        if (this != &other) {
            if (this->obj != nullptr) {
                auto& obj_pool = ObjectPool::instance();
                obj_pool.release(this->obj->get_object_id());
            }
            this->obj = other.obj;
            other.obj = nullptr;
        }
        return *this;
    }
} // namespace gkit::core
