#pragma once

#include <concepts>
#include <filesystem>

namespace gkit::resource {
    /** @brief CPU resource. Published instances are immutable and may be shared across threads. */
    class Resource {
    public:
        Resource() = default;
        explicit Resource(std::filesystem::path path);
        virtual ~Resource()                          = default;
        Resource(const Resource&)                    = delete;
        auto operator=(const Resource&) -> Resource& = delete;
        virtual auto load_from_file() -> void        = 0;
        auto get_path() const noexcept -> const std::filesystem::path& { return this->res_path; }
        auto available() const noexcept -> bool { return this->is_available; }

    protected:
        bool is_available = false;
        std::filesystem::path res_path;
    };

    template<class T>
    concept IsResource = std::derived_from<T, Resource>;
} // namespace gkit::resource
