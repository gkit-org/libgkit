#pragma once

#include "gkit/core/object.hpp"
#include "object_id.hpp"

#include <optional>
#include <utility>

namespace gkit::core {
    /**
     * @brief Move-only handle to a pool-backed Object instance.
     *
     * Every Object created through this class is registered in the internal
     * ObjectPool and owned by it until the handle is destroyed or reassigned.
     * The handle is move-only: ownership transfers on move, and a moved-from
     * handle is empty (get() == nullptr, get_id() unavailable).
     */
    class UniqueObject final {
        Object* obj = nullptr;
        ObjectId id;

        UniqueObject() = default;

    public:
        /// @brief Release the owned object back to the ObjectPool.
        ~UniqueObject() noexcept;

        UniqueObject(const UniqueObject&) noexcept   = delete;
        auto operator=(const UniqueObject&) noexcept = delete;

        /// @brief Transfer ownership from another handle.
        UniqueObject(UniqueObject&& other) noexcept;

        /// @brief Transfer ownership from another handle, releasing the current one.
        auto operator=(UniqueObject&& other) noexcept -> UniqueObject&;

        /// @brief The owned object pointer, or nullptr if the handle is empty.
        [[nodiscard]] auto get() -> Object* { return this->obj; }

        /// @brief The owned object pointer (const), or nullptr if the handle is empty.
        [[nodiscard]] auto get() const -> const Object* { return this->obj; }

        /// @brief The object id, or an unavailable id if the handle is empty.
        [[nodiscard]] inline auto get_id() const -> const ObjectId& { return this->id; }

        /**
         * @brief Create an object of type @p T and take ownership of it.
         * @tparam T The object type; must derive from Object.
         * @param args Arguments forwarded to T's constructor.
         * @return A handle owning the new object. Construction and allocation failures propagate.
         */
        template<IsObject T, class... Args>
        static auto create(Args&&... args) -> UniqueObject;

        /**
         * @brief Create an object by its registered class name.
         * @param class_name Name registered in ClassDB via regist<T>().
         * @return A handle owning the new object, or std::nullopt if the
         *         class is unknown or has no factory.
         */
        static auto create_with_classname(const std::string& class_name) -> std::optional<UniqueObject>;

    private:
        /// @brief Register an already-constructed object into the pool and wrap it in a handle.
        static auto from_raw_ptr(Object* obj) -> UniqueObject;
    };

    template<IsObject T, class... Args>
    auto UniqueObject::create(Args&&... args) -> UniqueObject {
        return from_raw_ptr(new T(std::forward<Args>(args)...));
    }
} // namespace gkit::core
