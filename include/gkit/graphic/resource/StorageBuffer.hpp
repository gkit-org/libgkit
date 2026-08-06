#pragma once

#include "gkit/graphic/resource/Buffer.hpp"

namespace gkit::graphic {

    /**
	 * @brief Storage buffer (placeholder, not implemented yet)
	 *
	 * Buffer readable/writable by compute shaders (SSBO). Once the resource
	 * module and backends are ready, extend this base class (bind to compute
	 * pipelines / material slots).
	 *
	 * TODO(future): implement the backend (opengl::StorageBuffer) and the Device
	 *               factory method create_storage_buffer().
	 */
    // class StorageBuffer : public Buffer {
    // public:
    //     ~StorageBuffer() override = default;
    // };

} // namespace gkit::graphic
