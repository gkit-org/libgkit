#pragma once

#include "utils/Singleton.hpp"
#include "resource.hpp"
#include <unordered_map>
#include <shared_mutex>
#include <optional>
#include <filesystem>
#include <memory>

namespace gkit::resource {
    class ResourceLoader : utils::Singleton<ResourceLoader> {
    public:
        ResourceLoader() = default;

    public: 
        template<IsResource T>
        auto load(std::filesystem::path path) -> std::optional<std::shared_ptr<T>> {
            auto cached_res = get_cache(path);
            if (cached_res.has_value()) {
                return std::static_pointer_cast<T>(cached_res);
            }

            auto loaded_res = std::make_shared(T());
            loaded_res->load_from_file();

            if (loaded_res->available()) {
                this->push_to_cache(loaded_res);
                return std::static_pointer_cast<T>(loaded_res);
            } else {
                return std::nullopt;
            }
        };
    private:
        std::shared_mutex cache_rw_mutex = std::shared_mutex();
        std::unordered_map<std::filesystem::path, std::shared_ptr<gkit::resource::Resource>> resource_cache 
                    = std::unordered_map<std::filesystem::path, std::shared_ptr<gkit::resource::Resource>>();

        auto push_to_cache(std::shared_ptr<gkit::resource::Resource> res) -> void;
        auto get_cache(std::filesystem::path path) -> std::optional<std::shared_ptr<gkit::resource::Resource>>;
    }; // class ResourceLoader
} // namespace gkit::resource