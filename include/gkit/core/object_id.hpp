#pragma once

#include "gkit/core/object.hpp"
#include "gkit/core/templates/gen_id.hpp"

#include <functional>
#include <type_traits>

namespace gkit::core {
    class ObjectIdTag;

    /**
     * @brief Object ID backed by GenId, giving each object a unique id with a
     *        monotonic generation to guard against stale references.
     */
    class ObjectId final : public templates::GenId<ObjectIdTag> {
    public:
        ObjectId() noexcept = default;

        explicit ObjectId(const templates::GenId<ObjectIdTag>& base) noexcept : templates::GenId<ObjectIdTag>(base) {}

        /** @brief Pin lifetime; does not synchronize the object's fields. */
        auto lock() const -> std::shared_ptr<Object>;

        template<IsObject T>
        auto lock() const -> std::shared_ptr<T> {
            return std::dynamic_pointer_cast<T>(this->lock());
        }

        /** @brief Borrow without pinning; requires external lifetime synchronization. */
        auto deref() const noexcept -> Object*;

        template<IsObject T>
        auto deref_as() const noexcept -> T*;
    };

    template<IsObject T>
    auto ObjectId::deref_as() const noexcept -> T* {
        if constexpr (std::is_same_v<T, Object>) {
            return this->deref();
        } else {
            return dynamic_cast<T*>(this->deref());
        }
    }
} // namespace gkit::core

template<>
struct std::hash<gkit::core::ObjectId> {
    auto operator()(const gkit::core::ObjectId& obj_id) const -> std::size_t {
        return std::hash<gkit::core::templates::GenId<gkit::core::ObjectIdTag>>{}(obj_id);
    }
};
