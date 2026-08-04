#pragma once

#include "gkit/core/object_id.hpp"

#include <concepts>
#include <string>

namespace gkit::core {
    class Object;
    class ObjectPool;

    template<class T>
    concept IsObject = std::derived_from<T, Object>;

    namespace reflect::detail {
        class ObjectNode;
    }

    /**
     * @brief The base object in libgkit,
     * and should create it with func @ref gkit::core::UniqueObject::create<T>(...);
     */
    class Object {
        friend class gkit::core::reflect::detail::ObjectNode;
        friend ObjectPool;

    public:
        Object()          = default;
        virtual ~Object() = default;

        /**
         * @brief Create a instance of the type which is based of class Object,
         * which will malloc memory with gkit memory manager fuction
         * @tparam T The type of the instance. It must be a class which is based of class Object.
         * @param name The name of the instance.
         * @return A unique pointer to the instance. If the type can't be created, return nullptr.
         */
        /* template<IsObject T, typename... Args>
        static auto create(Args&&...) noexcept -> UniqueObject; */

        virtual auto class_name() const -> std::string final;

        [[nodiscard]] auto get_object_id() const noexcept -> const ObjectId& { return this->obj_id; }

    protected:
        ObjectId obj_id;
    };
} // namespace gkit::core