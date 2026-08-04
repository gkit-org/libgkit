#include "gkit/core/object_pool.hpp"

#include "gkit/core/object.hpp"
#include "gkit/core/object_id.hpp"

namespace gkit::core {
    auto ObjectPool::release(const ObjectId& drop_id) noexcept -> void {
        if (!drop_id.available()) return;

        auto target_it = this->id_instance_map.find(drop_id);
        if (target_it == this->id_instance_map.end()) return;

        auto* drop_obj = target_it->second;
        this->id_instance_map.erase(target_it);
        delete drop_obj;

        auto& id_alloc = ObjectIdAllocator::instance();
        id_alloc.drop(drop_id);
    }

    auto ObjectPool::deref_from(const ObjectId& id) noexcept -> Object* {
        if (!id.available()) return nullptr;

        auto target_it = this->id_instance_map.find(id);
        if (target_it == this->id_instance_map.end()) return nullptr;

        return target_it->second;
    }
} // namespace gkit::core