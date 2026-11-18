#pragma once

#include "gkit/core/object.hpp"
#include "gkit/core/object_id.hpp"
#include "gkit/core/templates/singleton.hpp"

#include <mutex>
#include <optional>
#include <unordered_map>

namespace gkit::core {
    namespace detail {
        struct NodeState;
    }
    /**
     * @brief Internal registry mapping ObjectId to live Object instances.
     *
     * Owns every Object allocated through UniqueObject: acquire() mints the
     * id and takes ownership, release() deletes the object and recycles the
     * id. This class is an implementation detail and is only reachable from
     * within the library sources.
     */
    class ObjectPool : public gkit::core::templates::Singleton<ObjectPool> {
        friend gkit::core::templates::Singleton<ObjectPool>;
        std::mutex pool_mutex;
        std::unordered_map<ObjectId, std::shared_ptr<Object>> id_instance_map;

        ObjectPool() { (void)ObjectId::IdAllocator::instance(); }
        ~ObjectPool() = default;

    public:
        /** @brief Register a constructed object; allocation failure leaves the caller's shared owner intact. */
        auto acquire(std::shared_ptr<Object> obj) -> std::optional<ObjectId>;

        /**
         * @brief Unregister and delete the object behind @p drop_id.
         * @param drop_id A valid id previously returned by acquire().
         */
        auto release(const ObjectId& drop_id) noexcept -> void;

        /**
         * @brief Look up the object behind @p id.
         * @param id A previously acquired id.
         * @return The object pointer, or nullptr if @p id is invalid or unknown.
         */
        auto deref_from(const ObjectId& id) noexcept -> Object*;
        auto find_node(ObjectId id) -> detail::NodeState*;
        auto lock(const ObjectId& id) -> std::shared_ptr<Object>;
    };
} // namespace gkit::core
