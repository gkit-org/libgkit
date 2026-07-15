#include "gkit/resource/resource.hpp"

gkit::resource::Resource::Resource(ResourceId id, std::filesystem::path path) : res_id(id), res_path(std::move(path)) {}
