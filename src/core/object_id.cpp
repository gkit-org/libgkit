#include "gkit/core/object_id.hpp"

#include "object_pool.hpp"

namespace gkit::core {
    auto ObjectId::lock() const -> std::shared_ptr<Object> {
        return ObjectPool::instance().lock(*this);
    }

    auto ObjectId::deref() const noexcept -> Object* {
        if (!this->available()) return nullptr;
        return ObjectPool::instance().deref_from(*this);
    }
} // namespace gkit::core
