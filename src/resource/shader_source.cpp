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
            static constexpr std::pair<std::string_view, ShaderStage> MARKERS[] = {
                {"vertex", ShaderStage::Vertex},
                {"fragment", ShaderStage::Fragment},
                {"geometry", ShaderStage::Geometry},
                {"compute", ShaderStage::Compute},
                {"tess_control", ShaderStage::TessControl},
                {"tess_evaluation", ShaderStage::TessEvaluation},
            };
            for (const auto& [name, stage] : MARKERS) {
                if (name == marker) return stage;
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

    ShaderSource::ShaderSource(const std::filesystem::path& path) :
        gkit::resource::Resource(path), data(std::make_unique<Data>()) {}

    ShaderSource::ShaderSource(std::filesystem::path&& path) :
        gkit::resource::Resource(path), data(std::make_unique<Data>()) {}

    ShaderSource::~ShaderSource() = default;

    auto ShaderSource::load_from_file() -> void {
        this->is_available = false;
        this->data->full_source.clear();
        this->data->stage_sources.clear();
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

            auto first = line.find_first_not_of(" \t\r");
            if (first != std::string::npos && line.compare(first, 7, "#shader") == 0 &&
                (first + 7 == line.size() || line[first + 7] == ' ' || line[first + 7] == '\t')) {
                auto marker_start = line.find_first_not_of(" \t", first + 7);
                if (marker_start == std::string::npos) return;
                auto last  = line.find_last_not_of(" \t\r");
                auto stage = stage_from_marker(std::string_view(line).substr(marker_start, last - marker_start + 1));
                if (!stage || stage_streams.contains(*stage)) return;
                current_stage = stage;
                stage_streams.try_emplace(*stage);
                continue;
            }

            if (current_stage.has_value()) {
                stage_streams[*current_stage] << line << '\n';
            }
        }

        if (stream.bad() || stage_streams.empty()) return;
        for (const auto& [stage, source] : stage_streams) {
            if (source.str().find_first_not_of(" \t\r\n") == std::string::npos) return;
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

} // namespace gkit::resource
