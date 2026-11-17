#include "core/object_pool.hpp"

namespace gkit::core {
    auto ObjectPool::acquire(std::shared_ptr<Object> obj) -> std::optional<ObjectId> {
        if (!obj) return std::nullopt;
        auto id = ObjectId(ObjectId::IdAllocator::instance().new_one());
        try {
            std::lock_guard guard(this->pool_mutex);
            this->id_instance_map.emplace(id, std::move(obj));
        } catch (...) {
            ObjectId::IdAllocator::instance().drop(id);
            throw;
        }
        return id;
    }

    auto ObjectPool::release(const ObjectId& id) noexcept -> void {
        std::shared_ptr<Object> retired;
        {
            std::lock_guard guard(this->pool_mutex);
            auto it = this->id_instance_map.find(id);
            if (it == this->id_instance_map.end()) return;
            retired = std::move(it->second);
            this->id_instance_map.erase(it);
        }
        ObjectId::IdAllocator::instance().drop(id);
        // User destruction runs after unlocking the registry.
    }

    auto ObjectPool::lock(const ObjectId& id) -> std::shared_ptr<Object> {
        std::lock_guard guard(this->pool_mutex);
        auto it = this->id_instance_map.find(id);
        return it == this->id_instance_map.end() ? nullptr : it->second;
    }

    auto ObjectPool::deref_from(const ObjectId& id) noexcept -> Object* {
        return this->lock(id).get(); // Compatibility: caller must keep the owner alive.
    }
} // namespace gkit::core
