#include <gkit/resource/resource_loader.hpp>


auto gkit::resource::ResourceLoader::push_to_cache(std::shared_ptr<gkit::resource::Resource> res) -> void {
    if (res == nullptr && !res->available()) {
        return;
    }

    this->cache_rw_mutex.lock();
    this->resource_cache.insert(std::make_pair(res->get_path(), res));
}


auto gkit::resource::ResourceLoader::get_cache(std::filesystem::path path) -> std::optional<std::shared_ptr<gkit::resource::Resource>> {
    this->cache_rw_mutex.lock_shared();
    if (!this->resource_cache.contains(path)) {
        return std::nullopt;
    }

    return this->resource_cache.at(path);
}