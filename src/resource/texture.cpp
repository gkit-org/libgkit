#include "gkit/resource/texture.hpp"
#include "SDL3/SDL_surface.h"
#include "SDL3_image/SDL_image.h"
#include <memory>


namespace gkit::resource {
    struct Texture::Data {
        std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> surface;
        Data() : surface(nullptr, &SDL_DestroySurface) { }
    };
}


gkit::resource::Texture::Texture() : gkit::resource::Resource(), data(std::make_unique<Data>(Data())) { }


auto gkit::resource::Texture::load_from_file() -> void {
    if (this->res_path.empty()) {
        this->is_available = false;
        this->data->surface.reset(nullptr);
        return;
    }
    
    auto* surface_ptr = IMG_Load(this->res_path.string().c_str());
    if(surface_ptr == nullptr) {
        this->is_available = false;
        this->data->surface.reset(nullptr);
        return;
    }

    this->data->surface.reset(surface_ptr);
    this->is_available = true;
    return;
}