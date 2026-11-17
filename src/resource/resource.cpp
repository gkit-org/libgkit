#include "gkit/resource/resource.hpp"

namespace gkit::resource {
    Resource::Resource(std::filesystem::path path) : res_path(std::move(path)) {}
} // namespace gkit::resource
