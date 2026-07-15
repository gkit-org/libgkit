#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

namespace gkit::resource {

    /**
     * @brief Unique identifier for resources based on FNV-1a 64-bit hash
     *
     * Immutable, trivially copyable, and usable as a key in
     * unordered containers via std::hash<ResourceId>.
     */
    class ResourceId {
    public:
        using ValueType = std::uint64_t;

        constexpr ResourceId() noexcept = default;
        constexpr explicit ResourceId(ValueType value) noexcept : id(value) {}

        /**
         * @brief Create a ResourceId from a string using FNV-1a hash
         */
        [[nodiscard]] static constexpr auto from_string(const char* str) noexcept -> ResourceId {
            return ResourceId(fnv1a_hash(std::string_view(str)));
        }

        /**
         * @brief Create a ResourceId from a string view using FNV-1a hash
         */
        [[nodiscard]] static constexpr auto from_string(std::string_view str) noexcept -> ResourceId {
            return ResourceId(fnv1a_hash(str));
        }

        /**
         * @brief Get the underlying hash value
         */
        [[nodiscard]] constexpr auto value() const noexcept -> ValueType { return id; }

        /**
         * @brief Check if this ID is non-zero (valid)
         */
        [[nodiscard]] constexpr auto is_valid() const noexcept -> bool { return id != 0; }

        constexpr auto operator==(const ResourceId&) const noexcept -> bool                  = default;
        constexpr auto operator<=>(const ResourceId&) const noexcept -> std::strong_ordering = default;

        /**
         * @brief Debug string representation of the hash
         */
        [[nodiscard]] auto to_string() const -> std::string;

    private:
        ValueType id = 0;

        /**
         * @brief FNV-1a 64-bit hash implementation
         */
        [[nodiscard]] static constexpr auto fnv1a_hash(std::string_view str) noexcept -> ValueType {
            ValueType hash = 14695981039346656037ull;
            for (auto c : str) {
                hash ^= static_cast<unsigned char>(c);
                hash *= 1099511628211ull;
            }
            return hash;
        }
    };

} // namespace gkit::resource

template<>
struct std::hash<gkit::resource::ResourceId> {
    constexpr auto operator()(const gkit::resource::ResourceId& id) const noexcept -> std::size_t {
        return static_cast<std::size_t>(id.value());
    }
};
