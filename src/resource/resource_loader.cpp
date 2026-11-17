#include "gkit/resource/resource_loader.hpp"

namespace gkit::resource {
    auto ResourceLoader::clear() -> void {
        std::lock_guard guard(this->cache_mutex);
        ++this->epoch;
        this->cache.clear();
    }

    auto ResourceLoader::prune() -> void {
        std::lock_guard guard(this->cache_mutex);
        std::erase_if(this->cache, [](const auto& entry) {
            return entry.second.expired();
        });
    }
} // namespace gkit::resource
