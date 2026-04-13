#pragma once

#include <filesystem>
#include <vector>

namespace gkit::resource {
    class Resource {
    public:
        Resource() = default;
        Resource(std::filesystem::path path);
        Resource(std::filesystem::path&& path);
        virtual ~Resource() = default;
        Resource  operator=(Resource&)  = delete;
        Resource  operator=(Resource&&) = delete;

    protected:
        friend class ResourceLoader;
        /**
         * @brief Serialize the resource to a byte string
         * @note  It can use to save the resource to a file
         * @param void
         * @return std::vector<std::byte> the byte string
         */
        virtual auto parse() -> std::vector<std::byte> = 0;

        /**
         * @brief Dump the resource from a byte string
         * @note  It can use to load the resource from a file.
         *
         * And the information in byte string will be load into the object
         * @param data the byte string
         * @return void
         */
        virtual auto dump(std::vector<std::byte>) -> void = 0;

        /**
         * @brief Load the resource from a file from the res_path which store in the resource object
         * @param void
         * @return void
         */
        virtual auto load_from_file() -> void = 0;

    public:
        inline auto get_path() -> std::filesystem::path {
            return res_path;
        }

    protected:
        bool is_available = false;
        std::filesystem::path res_path = std::filesystem::path();

        inline auto available() -> bool {
            return is_available;
        }
    };

    template <typename T>
    concept IsResource = requires(T v) {
        std::is_base_of_v<gkit::resource::Resource, T>;
        v.load_from_file();
    };
}