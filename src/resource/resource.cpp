#include "gkit/resource/resource.hpp"


gkit::resource::Resource::Resource(std::filesystem::path path) : res_path(std::move(path)) { }
gkit::resource::Resource::Resource(std::filesystem::path&& path) : gkit::resource::Resource::Resource(path) { }

