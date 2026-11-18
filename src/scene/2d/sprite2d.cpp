#include "gkit/scene/2d/sprite2d.hpp"

#include "gkit/resource/resource_loader.hpp"

namespace gkit::scene {
    auto Sprite2D::set_texture(const std::filesystem::path& path) -> void {
        auto result = resource::ResourceLoader::instance().load<resource::Texture>(path);
        if (!result) throw std::runtime_error(path.string() + ": " + result.message);
        std::lock_guard guard(texture_mutex);
        texture = std::move(result.resource);
    }
} // namespace gkit::scene
