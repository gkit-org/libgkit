#include "core/object_pool.hpp"

#include "core/node_internal.hpp"

namespace gkit::core {
    auto ObjectPool::acquire(std::shared_ptr<Object> obj) -> std::optional<ObjectId> {
        if (!obj) return std::nullopt;
        auto id = ObjectId(ObjectId::IdAllocator::instance().new_one());
        try {
            std::lock_guard guard(this->pool_mutex);
            if (auto* node = dynamic_cast<Node*>(obj.get())) detail::NodeState::of(*node).id = id;
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

    // Caller holds the topology lock or is traversing an owned, stable running tree.
    auto ObjectPool::find_node(ObjectId id) -> detail::NodeState* {
        std::lock_guard guard(pool_mutex);
        auto it    = id_instance_map.find(id);
        auto* node = it == id_instance_map.end() ? nullptr : dynamic_cast<Node*>(it->second.get());
        if (!node) return nullptr;
        auto& state = detail::NodeState::of(*node);
        return state.retired ? nullptr : &state;
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
