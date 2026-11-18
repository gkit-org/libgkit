#pragma once

#include <SDL3/SDL_video.h>

#include <memory>
#include <stdexcept>
#include <thread>

namespace gkit::graphic::opengl {
    /** @brief Identity of one context, owned by the running application. */
    struct Context {
        SDL_GLContext native;
        std::thread::id thread = std::this_thread::get_id();

        auto is_current() const noexcept -> bool {
            return thread == std::this_thread::get_id() && SDL_GL_GetCurrentContext() == native;
        }
    };

    /** @brief Guard use and destruction of resources that may outlive their context. */
    class ContextResource {
    public:
        auto require_context() const -> void {
            if (!has_current_context()) throw std::logic_error("the resource's OpenGL context is not current");
        }

    protected:
        explicit ContextResource(const std::shared_ptr<const Context>& context) : context(context) {
            require_context();
        }
        auto has_current_context() const noexcept -> bool {
            const auto owner = context.lock();
            return owner && owner->is_current();
        }
        auto context_alive() const noexcept -> bool { return !context.expired(); }

    private:
        std::weak_ptr<const Context> context;
    };

    /** @brief Validate a frontend resource before using its backend object. */
    template<class T, class Resource>
    auto require_resource(const Resource& resource) -> const T& {
        auto* native = dynamic_cast<const T*>(&resource);
        if (!native) throw std::invalid_argument("resource belongs to another graphics backend");
        native->require_context();
        return *native;
    }
} // namespace gkit::graphic::opengl
