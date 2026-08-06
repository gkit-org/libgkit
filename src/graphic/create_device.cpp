#include "gkit/graphic/render/RenderDevice.hpp"
#include "graphic/backend/opengl/Device.hpp"

namespace gkit::graphic {

    auto create_device(Backend backend) -> std::unique_ptr<RenderDevice> {
        switch (backend) {
        case Backend::OpenGL:
            return std::make_unique<opengl::Device>();
        }
        // future: case Backend::Vulkan: return std::make_unique<vulkan::Device>();
        return nullptr;
    }

} // namespace gkit::graphic
