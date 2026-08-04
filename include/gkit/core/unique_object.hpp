#pragma once

#include "gkit/core/object_pool.hpp"
#include "gkit/core/object.hpp"
#include "object_id.hpp"

#include <optional>
#include <utility>

namespace gkit::core {
    class UniqueObject final {
        Object* obj = nullptr;

        UniqueObject() = default;

    public:
        ~UniqueObject() noexcept;
        UniqueObject(const UniqueObject&) noexcept = delete;
        UniqueObject(UniqueObject&& other) noexcept;
        auto operator=(const UniqueObject&) noexcept = delete;
        auto operator=(UniqueObject&& other) noexcept -> UniqueObject&;

        [[nodiscard]] auto get() -> Object* { return this->obj; }
        [[nodiscard]] auto get() const -> const Object* { return this->obj; }
        [[nodiscard]] auto get_id() const noexcept -> const ObjectId&;

        template<IsObject T, class... Args>
        static auto create(Args&&... args) noexcept -> UniqueObject;
        static auto create_with_classname(const std::string& class_name) noexcept -> std::optional<UniqueObject>;
    };

    template<IsObject T, class... Args>
    auto UniqueObject::create(Args&&... args) noexcept -> UniqueObject {
        UniqueObject uobj;
        auto& obj_pool = ObjectPool::instance();
        auto obj_opt   = obj_pool.create<T>(std::forward(args)...);
        if (obj_opt.has_value()) {
            uobj.obj = obj_opt->second;
        }

        return uobj;
    }
} // namespace gkit::core