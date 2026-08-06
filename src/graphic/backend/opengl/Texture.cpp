#include "graphic/backend/opengl/Texture.hpp"

//#include <stb_image.h>
#include <glad/gl.h>

//#include <iostream>

gkit::graphic::opengl::Texture::Texture(const std::string& path, gkit::graphic::TextureType type) :
    renderer_id(0), filepath(path), local_buffer(nullptr), width(0), height(0), bpp(0), type(type) {
    // Framebuffer textures need a real GL texture object with storage to be a
    // valid FBO color attachment. Image loading (stb) is stubbed out for now,
    // but off-screen textures only need empty renderable storage.
    if (this->type == gkit::graphic::TextureType::TextureFramebuffer) {
        this->width  = gkit::graphic::SCR_WIDTH;
        this->height = gkit::graphic::SCR_HEIGHT;
        glGenTextures(1, &this->renderer_id);
        glBindTexture(GL_TEXTURE_2D, this->renderer_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

auto gkit::graphic::opengl::Texture::set_size(int width, int height) -> void {
    // Only framebuffer textures are (re)allocated on attach; loaded images must
    // keep their pixel data.
    if (this->type != gkit::graphic::TextureType::TextureFramebuffer) {
        return;
    }
    if (this->width == width && this->height == height) {
        return;
    }
    this->width  = width;
    this->height = height;
    glBindTexture(GL_TEXTURE_2D, this->renderer_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

gkit::graphic::opengl::Texture::~Texture() {
    delete[] this->local_buffer;
    if (this->renderer_id != 0) {
        glDeleteTextures(1, &this->renderer_id);
        this->renderer_id = 0;
    }
}

gkit::graphic::opengl::Texture::Texture(Texture&& other) noexcept :
    graphic::Texture(std::move(other)), renderer_id(other.renderer_id), filepath(std::move(other.filepath)),
    local_buffer(other.local_buffer), width(other.width), height(other.height), bpp(other.bpp), type(other.type) {
    other.renderer_id  = 0;
    other.local_buffer = nullptr;
}

auto gkit::graphic::opengl::Texture::operator=(Texture&& other) noexcept -> Texture& {
    if (this != &other) {
        glDeleteTextures(1, &this->renderer_id);
        graphic::Texture::operator=(std::move(other));
        this->renderer_id  = other.renderer_id;
        this->filepath     = std::move(other.filepath);
        this->local_buffer = other.local_buffer;
        this->width        = other.width;
        this->height       = other.height;
        this->bpp          = other.bpp;
        this->type         = other.type;
        other.renderer_id  = 0;
        other.local_buffer = nullptr;
    }
    return *this;
}

auto gkit::graphic::opengl::Texture::bind(unsigned int slot) const -> void {
    if (this->type == gkit::graphic::TextureType::TextureCubeMap) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->renderer_id);
    } else if (this->type == gkit::graphic::TextureType::Texture2D ||
               this->type == gkit::graphic::TextureType::TextureFramebuffer) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, this->renderer_id);
    }
}

auto gkit::graphic::opengl::Texture::unbind() const -> void {
    if (this->type == gkit::graphic::TextureType::TextureCubeMap) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    } else if (this->type == gkit::graphic::TextureType::Texture2D ||
               this->type == gkit::graphic::TextureType::TextureFramebuffer) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
