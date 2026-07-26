#pragma once

#include <concepts>
#include <memory>
#include <utility>

namespace gkit::core::scene {
    class Object;

    template<class T>
    concept IsObject = std::derived_from<T, Object>;

    /**
     * @brief The base object in libgkit,
     * and should create it with func @ref gkit::core::scene::Object::create<T>().
     */
    class Object {
    protected:
        Object() = default;

    public:
        virtual ~Object() = default;

        /**
         * @brief Create a instance of the type which is based of class Object,
         * which will malloc memory with gkit memory manager fuction
         * @tparam T The type of the instance. It must be a class which is based of class Object.
         * @param name The name of the instance.
         * @return A unique pointer to the instance. If the type can't be created, return nullptr.
         */
        template<IsObject T, typename... Args>
        static auto create(Args&&...) noexcept -> std::unique_ptr<T>;
    };

    template<IsObject T, typename... Args>
    auto Object::create(Args&&... args) noexcept -> std::unique_ptr<T> {
        try {
            auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
            return ptr;
        } catch (...) {
            return nullptr;
        }
    }
} // namespace gkit::core::scene