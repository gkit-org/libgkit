#include "gkit/resource/shader_source.hpp"

#include <fstream>
#include <optional>
#include <sstream>
#include <unordered_map>

namespace gkit::resource {

    namespace {

        /**
         * @brief Map marker string to ShaderStage enum
         */
        auto stage_from_marker(std::string_view marker) noexcept -> std::optional<ShaderStage> {
            static const std::unordered_map<std::string_view, ShaderStage> MARKER_MAP = {
                {"vertex", ShaderStage::Vertex},
                {"fragment", ShaderStage::Fragment},
                {"geometry", ShaderStage::Geometry},
                {"compute", ShaderStage::Compute},
                {"tess_control", ShaderStage::TessControl},
                {"tess_evaluation", ShaderStage::TessEvaluation},
            };

            auto it = MARKER_MAP.find(marker);
            if (it != MARKER_MAP.end()) {
                return it->second;
            }
            return std::nullopt;
        }

    } // anonymous namespace

    /**
     * @brief Internal data for ShaderSource (pimpl idiom)
     *
     * Holds the raw source text alongside per-stage source code
     * parsed from #shader markers.
     */
    struct ShaderSource::Data {
        std::string full_source; // Complete file content
        std::unordered_map<ShaderStage, std::string> stage_sources; // Per-stage source code
    };

    ShaderSource::ShaderSource() : gkit::resource::Resource(), data(std::make_unique<Data>()) {}

    ShaderSource::ShaderSource(ResourceId id, std::filesystem::path path) :
        gkit::resource::Resource(id, std::move(path)), data(std::make_unique<Data>()) {}

    ShaderSource::~ShaderSource() = default;

    auto ShaderSource::load_from_file() -> void {
        if (this->res_path.empty()) {
            this->is_available = false;
            this->data->full_source.clear();
            this->data->stage_sources.clear();
            return;
        }

        std::ifstream stream(this->res_path);
        if (!stream.is_open()) {
            this->is_available = false;
            this->data->full_source.clear();
            this->data->stage_sources.clear();
            return;
        }

        // Read the entire file and parse stage markers
        std::string line;
        std::stringstream full_ss;
        std::unordered_map<ShaderStage, std::stringstream> stage_streams;

        std::optional<ShaderStage> current_stage = std::nullopt;

        while (std::getline(stream, line)) {
            full_ss << line << '\n';

            // Check for stage marker: #shader <stage_name>
            if (line.find("#shader") != std::string::npos) {
                // Extract the stage name after "#shader "
                auto pos = line.find("#shader");
                pos += 8; // skip "#shader "

                // Skip whitespace
                while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t')) {
                    ++pos;
                }

                auto marker   = std::string_view(line).substr(pos);
                current_stage = stage_from_marker(marker);

                if (!current_stage.has_value()) {
                    // Unknown stage marker — reset to no stage, skip these lines
                    // until the next marker
                }
                continue;
            }

            if (current_stage.has_value()) {
                stage_streams[*current_stage] << line << '\n';
            }
        }

        this->data->full_source = full_ss.str();

        // Transfer stage streams into the map
        this->data->stage_sources.clear();
        for (auto& [stage, ss] : stage_streams) {
            auto src = ss.str();
            if (!src.empty()) {
                this->data->stage_sources.emplace(stage, std::move(src));
            }
        }

        this->is_available = !this->data->full_source.empty();
    }

    auto ShaderSource::source() const noexcept -> std::string_view {
        return this->data->full_source;
    }

    auto ShaderSource::get_source(ShaderStage stage) const -> std::string_view {
        auto it = this->data->stage_sources.find(stage);
        if (it != this->data->stage_sources.end()) {
            return it->second;
        }
        return {};
    }

    auto ShaderSource::has_stage(ShaderStage stage) const noexcept -> bool {
        return this->data->stage_sources.contains(stage);
    }

    auto ShaderSource::stages() const noexcept -> const std::unordered_map<ShaderStage, std::string>& {
        return this->data->stage_sources;
    }

    auto ShaderSource::get_path() const noexcept -> const std::filesystem::path& {
        return this->res_path;
    }

    auto ShaderSource::is_loaded() const noexcept -> bool {
        return this->is_available;
    }

    auto ShaderSource::parse() -> std::vector<std::byte> {
        // Serialization of shader source is not currently supported.
        // ShaderSource is a read-only resource type.
        return {};
    }

    auto ShaderSource::dump(std::vector<std::byte> /*data*/) -> void {
        // Deserialization of shader source is not currently supported.
        // ShaderSource is a read-only resource type.
    }

} // namespace gkit::resource
