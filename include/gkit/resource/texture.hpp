#pragma once

#include "gkit/resource/resource.hpp"
#include <filesystem>
#include <memory>

namespace gkit::resource {
    class Texture : protected gkit::resource::Resource {
    public:
        Texture();
        Texture(std::filesystem::path   path);
        Texture(std::filesystem::path&& path);

    protected:
        auto load_from_file() -> void override;
    
    private:
        struct Data;
        std::unique_ptr<Data> data;
    }; // class Texture
} // namespace gkit::resource