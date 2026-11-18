#include "gkit/core/unique_object.hpp"

#include "core/node_internal.hpp"
#include "gkit/core/object_id.hpp"
#include "gkit/core/reflect/registry.hpp"
#include "object_pool.hpp"

#include <string>

namespace gkit::core {
    auto UniqueObject::create_with_classname(const std::string& class_name) -> std::optional<UniqueObject> {
        return reflect::ClassDB::instance().create(class_name);
    }

    auto UniqueObject::from_raw_ptr(Object* obj) -> UniqueObject {
        UniqueObject uobj;
        if (obj == nullptr) return uobj;

        auto owned  = std::shared_ptr<Object>(obj);
        auto& pool  = ObjectPool::instance();
        auto id_opt = pool.acquire(owned);
        if (id_opt) {
            uobj.id  = *id_opt;
            uobj.obj = owned.get();
        }
        return uobj;
    }

    UniqueObject::UniqueObject(UniqueObject&& other) noexcept : id(other.id) {
        this->obj = other.obj;
        other.obj = nullptr;
        // Reset the source id so the moved-from handle releases nothing.
        other.id = ObjectId();
    }

    UniqueObject::~UniqueObject() noexcept {
        if (auto* node = dynamic_cast<Node*>(obj)) detail::NodeState::of(*node).retire();
        if (obj) ObjectPool::instance().release(id);
    }

    auto UniqueObject::operator=(UniqueObject&& other) noexcept -> UniqueObject& {
        if (this != &other) {
            UniqueObject retired(std::move(*this));
            this->id  = other.id;
            this->obj = other.obj;
            other.obj = nullptr;
            other.id  = ObjectId();
        }
        return *this;
    }
} // namespace gkit::core
