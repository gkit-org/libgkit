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

        /** Pin the matching object for this call. References must stay inside the callback. */
        template<IsObject T = Object, class Fn>
        auto with(Fn&& callback) const -> bool {
            auto pinned  = pin();
            auto* object = dynamic_cast<T*>(pinned.get());
            if (!object) return false;
            std::invoke(std::forward<Fn>(callback), *object);
            return true;
        }

        /** @brief Borrow without pinning; requires external lifetime synchronization. */
        auto deref() const noexcept -> Object*;

        template<IsObject T>
        auto deref_as() const noexcept -> T*;

    private:
        auto pin() const -> std::shared_ptr<Object>;
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
