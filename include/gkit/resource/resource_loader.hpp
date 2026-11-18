#pragma once

#include "gkit/core/templates/singleton.hpp"
#include "gkit/resource/resource.hpp"

#include <memory>
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
        struct Impl;
        std::unique_ptr<Impl> impl;
        ResourceLoader();
        using Decoder = std::shared_ptr<Resource> (*)(const std::filesystem::path&);
        auto load_file(const std::filesystem::path& path, std::type_index type, Decoder decode) -> LoadResult<Resource>;

    public:
        ~ResourceLoader();
        /** @brief Merge concurrent loads of the same resource and preserve decoding errors. */
        template<IsResource T>
        auto load(const std::filesystem::path& path) -> LoadResult<T> {
            auto result =
                load_file(path, typeid(T), [](const std::filesystem::path& file) -> std::shared_ptr<Resource> {
                    return std::make_shared<T>(file);
                });
            return {std::dynamic_pointer_cast<const T>(result.resource), result.error, std::move(result.message)};
        }
        /** @brief Existing handles and in-flight requests remain valid. */
        auto clear() -> void;
        auto prune() -> void;
    };
} // namespace gkit::resource
