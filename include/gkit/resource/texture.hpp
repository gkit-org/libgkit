#pragma once

#include "gkit/resource/resource.hpp"

#include <cstddef>
#include <memory>
#include <span>

namespace gkit::resource {
    /** @brief Decoded CPU image in RGBA byte order. No graphics context is required. */
    class Texture : public Resource {
    public:
        Texture();
        explicit Texture(std::filesystem::path path);
        ~Texture() override;

        auto load_from_file() -> void override;
        [[nodiscard]] auto width() const noexcept -> int;
        [[nodiscard]] auto height() const noexcept -> int;
        [[nodiscard]] auto pitch() const noexcept -> int;
        [[nodiscard]] auto pixels() const noexcept -> std::span<const std::byte>;

    private:
        struct Data;

        std::unique_ptr<Data> data;
    };
} // namespace gkit::resource
