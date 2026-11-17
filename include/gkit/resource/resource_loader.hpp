#pragma once

#include "gkit/core/templates/singleton.hpp"
#include "gkit/resource/resource.hpp"

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
    enum class LoadError {
        None,
        Path,
        Read,
        Decode,
        Recursive
    };

    template<class T>
    struct LoadResult {
        std::shared_ptr<const T> resource;
        LoadError error = LoadError::None;
        std::string message;
        explicit operator bool() const noexcept { return this->resource != nullptr; }
    };

    class ResourceLoader : public core::templates::Singleton<ResourceLoader> {
        friend class core::templates::Singleton<ResourceLoader>;
        ResourceLoader() = default;
        using Key        = std::pair<std::filesystem::path, std::type_index>;
        using Result     = LoadResult<Resource>;
        struct Pending {
            std::shared_future<Result> future;
            std::thread::id thread;
        };
        std::mutex cache_mutex;
        std::map<Key, std::weak_ptr<const Resource>> cache;
        std::map<Key, Pending> pending;
        std::uint64_t epoch = 0;

    public:
        /** @brief Merge concurrent loads; failed loads can be retried. Allocation failures propagate. */
        template<IsResource T>
        auto load_result(const std::filesystem::path& path) -> LoadResult<T> {
            std::error_code error;
            auto absolute = std::filesystem::absolute(path, error);
            if (error || path.empty()) return {nullptr, LoadError::Path, "invalid resource path"};
            auto normalized = std::filesystem::weakly_canonical(absolute, error);
            if (error) return {nullptr, LoadError::Path, error.message()};
            Key key{normalized, typeid(T)};
            std::shared_future<Result> future;
            std::shared_ptr<std::promise<Result>> promise;
            std::uint64_t load_epoch;
            {
                std::lock_guard guard(this->cache_mutex);
                if (auto it = this->cache.find(key); it != this->cache.end()) {
                    if (auto resource = it->second.lock()) {
                        return {std::dynamic_pointer_cast<const T>(resource)};
                    }
                }
                if (auto it = this->pending.find(key); it != this->pending.end()) {
                    if (it->second.thread == std::this_thread::get_id()) {
                        return {nullptr, LoadError::Recursive, "recursive resource load"};
                    }
                    future = it->second.future;
                } else {
                    promise = std::make_shared<std::promise<Result>>();
                    future  = promise->get_future().share();
                    this->pending.emplace(key, Pending{future, std::this_thread::get_id()});
                }
                load_epoch = this->epoch;
            }
            if (promise) {
                try {
                    Result result;
                    if (!std::filesystem::is_regular_file(normalized, error) || error ||
                        !std::ifstream(normalized, std::ios::binary).is_open()) {
                        result = {nullptr, LoadError::Read, "resource is not a readable regular file"};
                    } else {
                        auto resource = std::make_shared<T>(normalized);
                        resource->load_from_file();
                        if (resource->available())
                            result.resource = std::move(resource);
                        else
                            result = {nullptr, LoadError::Decode, "invalid resource data"};
                    }
                    std::lock_guard guard(this->cache_mutex);
                    if (result.resource && load_epoch == this->epoch) this->cache[key] = result.resource;
                    promise->set_value(std::move(result));
                    this->pending.erase(key);
                } catch (...) {
                    std::lock_guard guard(this->cache_mutex);
                    promise->set_exception(std::current_exception());
                    this->pending.erase(key);
                }
            }
            auto result = future.get();
            return {std::dynamic_pointer_cast<const T>(result.resource), result.error, result.message};
        }

        template<IsResource T>
        auto load(const std::filesystem::path& path) -> std::optional<std::shared_ptr<const T>> {
            auto result = this->load_result<T>(path);
            if (!result) return std::nullopt;
            return result.resource;
        }

        /** @brief Existing handles and in-flight requests remain valid. */
        auto clear() -> void;
        auto prune() -> void;
    };
} // namespace gkit::resource
