#include "gkit/resource/texture.hpp"

#include <SDL3/SDL_surface.h>

#include <SDL3_image/SDL_image.h>

namespace gkit::resource {
    struct Texture::Data {
        std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> surface{nullptr, SDL_DestroySurface};
    };
    Texture::Texture() : data(std::make_unique<Data>()) {}
    Texture::Texture(std::filesystem::path path) : Resource(std::move(path)), data(std::make_unique<Data>()) {}
    Texture::~Texture() = default;

    auto Texture::load_from_file() -> void {
        this->is_available = false;
        this->data->surface.reset();
        auto source = std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>(
            IMG_Load(this->res_path.string().c_str()), SDL_DestroySurface);
        if (!source) return;
        this->data->surface.reset(SDL_ConvertSurface(source.get(), SDL_PIXELFORMAT_RGBA32));
        this->is_available = this->data->surface != nullptr;
    }
    auto Texture::width() const noexcept -> int {
        return this->available() ? this->data->surface->w : 0;
    }
    auto Texture::height() const noexcept -> int {
        return this->available() ? this->data->surface->h : 0;
    }
    auto Texture::pitch() const noexcept -> int {
        return this->available() ? this->data->surface->pitch : 0;
    }
    auto Texture::pixels() const noexcept -> std::span<const std::byte> {
        if (!this->available()) return {};
        return {static_cast<const std::byte*>(this->data->surface->pixels),
                static_cast<std::size_t>(this->pitch()) * this->height()};
    }
} // namespace gkit::resource
