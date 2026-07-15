#pragma once

#include "gkit/core/scene/singleton.hpp"
#include "gkit/resource/resource.hpp"
#include "gkit/resource/resource_id.hpp"

#include <filesystem>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <unordered_map>

namespace gkit::resource {
    class ResourceManager : public core::scene::Singleton<ResourceManager> {
        friend class core::scene::Singleton<ResourceManager>;

    private:
        ResourceManager() = default;

    public:
        /**
         * @brief Load a resource from a file path
         *
         * The path is automatically hashed to generate a ResourceId.
         * If already cached (same path), returns the existing ID
         * without loading again.
         *
         * @tparam T Resource type (must satisfy IsResource concept)
         * @param file_path Path to the resource file on disk
         * @return The resource's unique ID, or nullopt if loading failed
         */
        template<IsResource T>
        auto load(const std::filesystem::path& file_path) -> std::optional<ResourceId> {
            auto id = ResourceId::from_string(file_path.string());

            auto cached_res = get_cache(id);
            if (cached_res.has_value()) {
                return id;
            }

            auto loaded_res = std::make_shared<T>(id, file_path);
            loaded_res->load_from_file();

            if (loaded_res->available()) {
                this->push_to_cache(loaded_res);
                return id;
            }

            return std::nullopt;
        };

        /**
         * @brief Retrieve a cached resource by its ID
         *
         * @tparam T Resource type
         * @param id Resource identifier returned by load()
         * @return shared_ptr to the resource, or nullopt if not cached
         */
        template<IsResource T>
        auto get(ResourceId id) -> std::optional<std::shared_ptr<T>> {
            auto cached_res = get_cache(id);
            if (!cached_res.has_value()) {
                return std::nullopt;
            }

            auto target_res = std::dynamic_pointer_cast<T>(cached_res.value());
            if (target_res == nullptr) {
                return std::nullopt;
            }

            return target_res;
        };

    private:
        std::shared_mutex cache_rw_mutex{};
        std::unordered_map<ResourceId, std::shared_ptr<gkit::resource::Resource>> resource_cache{};

        auto push_to_cache(const std::shared_ptr<gkit::resource::Resource>& res) -> void;
        auto get_cache(ResourceId id) -> std::optional<std::shared_ptr<gkit::resource::Resource>>;
    }; // class ResourceManager
} // namespace gkit::resource
