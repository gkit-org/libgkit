#pragma once

#include "gkit/resource/resource_id.hpp"

#include <filesystem>
#include <vector>

namespace gkit::resource {
    class Resource {
    public:
        Resource() = default;
        Resource(ResourceId id, std::filesystem::path path);
        virtual ~Resource()             = default;
        Resource(const Resource&)       = delete;
        Resource(Resource&&)            = delete;
        auto operator=(const Resource&) = delete;
        auto operator=(Resource&&)      = delete;

    protected:
        friend class ResourceManager;

        virtual auto parse() -> std::vector<std::byte>    = 0;
        virtual auto dump(std::vector<std::byte>) -> void = 0;
        virtual auto load_from_file() -> void             = 0;

    public:
        [[nodiscard]] auto get_id() const noexcept -> ResourceId { return res_id; }
        [[nodiscard]] auto get_path() const noexcept -> const std::filesystem::path& { return res_path; }

    protected:
        bool is_available = false;
        ResourceId res_id;
        std::filesystem::path res_path = std::filesystem::path();

        [[nodiscard]] auto available() -> bool { return is_available; }
    };

    template<typename T>
    concept IsResource = requires(T v) {
        { std::is_base_of_v<gkit::resource::Resource, T> } -> std::convertible_to<bool>;
        { v.load_from_file() } -> std::convertible_to<void>;
    };
} // namespace gkit::resource
