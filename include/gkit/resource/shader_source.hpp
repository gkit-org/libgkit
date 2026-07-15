#pragma once

#include "gkit/resource/resource.hpp"
#include "gkit/resource/resource_id.hpp"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace gkit::resource {

    /**
     * @brief Shader stage type enumeration
     *
     * Defines the recognized shader pipeline stages.
     * Vertex and Fragment are fully supported; additional
     * stages may be added in the future as needed.
     */
    enum class ShaderStage : std::uint8_t {
        Vertex,
        Fragment,

        // --- Extension slots for future pipeline stages ---
        Geometry,
        Compute,
        TessControl,
        TessEvaluation,
    };

    /**
     * @brief Shader source resource managed by the resource system
     *
     * Represents shader source code loaded from a file through
     * the ResourceManager. A single file may contain multiple stages
     * separated by markers (e.g. #shader vertex / #shader fragment),
     * with each stage's source accessible independently.
     *
     * @note resource::ShaderSource owns the source data; the actual
     *       compilation and uniform management is handled by
     *       gkit::graphic::Shader.
     */
    class ShaderSource : public gkit::resource::Resource {
    public:
        ShaderSource();
        ShaderSource(ResourceId id, std::filesystem::path path);
        ~ShaderSource() override;

        /**
         * @brief Get the full shader source code (all stages combined)
         * @return String view of the complete source text
         */
        [[nodiscard]] auto source() const noexcept -> std::string_view;

        /**
         * @brief Get source code for a specific shader stage
         * @param stage The shader stage to query
         * @return String view of the stage's source, or empty if not present
         */
        [[nodiscard]] auto get_source(ShaderStage stage) const -> std::string_view;

        /**
         * @brief Check if this shader contains a specific stage
         */
        [[nodiscard]] auto has_stage(ShaderStage stage) const noexcept -> bool;

        /**
         * @brief Get all available stages in this shader source
         */
        [[nodiscard]] auto stages() const noexcept -> const std::unordered_map<ShaderStage, std::string>&;

        /**
         * @brief Get the file path of this shader source
         */
        [[nodiscard]] auto get_path() const noexcept -> const std::filesystem::path&;

        /**
         * @brief Check if the shader source was successfully loaded
         */
        [[nodiscard]] auto is_loaded() const noexcept -> bool;

        auto load_from_file() -> void override;

    protected:
        auto parse() -> std::vector<std::byte> override;
        auto dump(std::vector<std::byte>) -> void override;

    private:
        struct Data;
        std::unique_ptr<Data> data;
    };

} // namespace gkit::resource
