#pragma once

#include "gkit/graphic/resource/buffer.hpp"
#include "gkit/math/matrix3.hpp"
#include "gkit/math/matrix4.hpp"
#include "gkit/math/vector3.hpp"
#include "gkit/math/vector4.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace gkit::graphic {

    /**
     * @brief A single uniform value (type-erased)
     */
    using UniformValue = std::variant<int, float, math::Vector3, math::Vector4, math::Matrix3, math::Matrix4>;

    /**
     * @brief Simple uniform set (value-by-value assignment)
     *
     * Simple path: the command carries a name→value list, and the executor
     * calls set_uniform_* for each entry.
     */
    struct UniformData {
        std::vector<std::pair<std::string, UniformValue>> values;
    };

    /**
     * @brief UBO block reference (bulk upload)
     *
     * Batch path: the command carries a reference to the user's parameter
     * struct, and the executor uploads the whole block at once.
     * Holds a reference, does not own — the user struct must stay alive until
     * flush finishes (lifetime contract).
     */
    struct UboBlock {
        /** @brief Pointer to the user struct (e.g. SceneParams). */
        const void* data = nullptr;
        /** @brief Size of the struct in bytes. */
        size_t size = 0;
        /** @brief UBO binding point. */
        uint32_t binding = 0;
    };

    /**
     * @brief Uniform buffer object (UBO) base class (placeholder)
     *
     * Buffer that stores uniform data (UBO) for batching shader constants.
     * Backend (opengl::UniformBuffer) + Device factory not implemented yet.
     */
    // class UniformBuffer : public Buffer {
    // public:
    //     ~UniformBuffer() override = default;
    // };

} // namespace gkit::graphic
