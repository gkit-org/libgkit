#include "gkit/resource/resource_loader.hpp"

#include <cstdint>
#include <fstream>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <typeindex>

namespace gkit::resource {
    struct ResourceLoader::Impl {
        using Key    = std::pair<std::filesystem::path, std::type_index>;
        using Result = LoadResult<Resource>;
        struct Pending {
            std::shared_future<Result> future;
            std::thread::id thread;
        };
        std::mutex cache_mutex;
        std::map<Key, std::weak_ptr<const Resource>> cache;
        std::map<Key, Pending> pending;
        std::uint64_t epoch = 0;
    };

    ResourceLoader::ResourceLoader() : impl(std::make_unique<Impl>()) {}


    ResourceLoader::~ResourceLoader() = default;


    auto ResourceLoader::load_file(const std::filesystem::path& path, std::type_index type, Decoder decode)
        -> LoadResult<Resource> {
        std::error_code error;
        auto absolute = std::filesystem::absolute(path, error);
        if (error || path.empty()) return {nullptr, LoadError::Path, "invalid resource path"};
        auto normalized = std::filesystem::weakly_canonical(absolute, error);
        if (error) return {nullptr, LoadError::Path, error.message()};
        Impl::Key key{normalized, type};
        std::shared_future<Impl::Result> future;
        std::optional<std::promise<Impl::Result>> promise;
        std::uint64_t load_epoch;
        {
            std::lock_guard guard(impl->cache_mutex);
            if (auto it = impl->cache.find(key); it != impl->cache.end()) {
                if (auto resource = it->second.lock()) {
                    return {std::move(resource)};
                }
            }
            if (auto it = impl->pending.find(key); it != impl->pending.end()) {
                if (it->second.thread == std::this_thread::get_id()) {
                    return {nullptr, LoadError::Recursive, "recursive resource load"};
                }
                future = it->second.future;
            } else {
                promise.emplace();
                future = promise->get_future().share();
                impl->pending.emplace(key, Impl::Pending{future, std::this_thread::get_id()});
            }
            load_epoch = impl->epoch;
        }
        if (promise) {
            try {
                Impl::Result result;
                if (!std::filesystem::is_regular_file(normalized, error) || error ||
                    !std::ifstream(normalized, std::ios::binary).is_open()) {
                    result = {nullptr, LoadError::Read, "resource is not a readable regular file"};
                } else {
                    auto resource = decode(normalized);
                    resource->load_from_file();
                    if (resource->available())
                        result.resource = std::move(resource);
                    else
                        result = {nullptr, LoadError::Decode, "invalid resource data"};
                }
                std::lock_guard guard(impl->cache_mutex);
                if (result.resource && load_epoch == impl->epoch) impl->cache[key] = result.resource;
                promise->set_value(std::move(result));
                impl->pending.erase(key);
            } catch (...) {
                std::lock_guard guard(impl->cache_mutex);
                promise->set_exception(std::current_exception());
                impl->pending.erase(key);
            }
        }
        return future.get();
    }


    auto ResourceLoader::clear() -> void {
        std::lock_guard guard(impl->cache_mutex);
        ++impl->epoch;
        impl->cache.clear();
    }


    auto ResourceLoader::prune() -> void {
        std::lock_guard guard(impl->cache_mutex);
        std::erase_if(impl->cache, [](const auto& entry) -> bool {
            return entry.second.expired();
        });
    }
} // namespace gkit::resource
