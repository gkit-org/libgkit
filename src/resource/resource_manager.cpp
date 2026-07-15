#include <mutex>

#include <gkit/resource/resource_manager.hpp>

auto gkit::resource::ResourceManager::push_to_cache(const std::shared_ptr<gkit::resource::Resource>& res) -> void {
    if (res == nullptr || !res->available()) {
        return;
    }

    std::unique_lock lock(this->cache_rw_mutex);
    this->resource_cache[res->get_id()] = res;
}

auto gkit::resource::ResourceManager::get_cache(ResourceId id)
    -> std::optional<std::shared_ptr<gkit::resource::Resource>> {
    std::shared_lock lock(this->cache_rw_mutex);
    auto it = this->resource_cache.find(id);
    if (it == this->resource_cache.end()) {
        return std::nullopt;
    }

    return it->second;
}
