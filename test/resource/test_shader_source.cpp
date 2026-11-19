#include "test_utils.hpp"

#include <filesystem>
#include <string_view>

#include <gkit/resource/resource_loader.hpp>
#include <gkit/resource/shader_source.hpp>

namespace fs = std::filesystem;

namespace {

    /**
     * @brief Get path to the test resource directory (same folder as this source file)
     */
    auto resource_dir() -> fs::path {
        return fs::path(__FILE__).parent_path();
    }

} // anonymous namespace

auto test_load_vertex_and_fragment() -> bool {
    gkit::test::logln("=== Test: Load Vertex and Fragment Shader ===");

    const auto path = resource_dir() / "basic.shader";
    auto result     = gkit::resource::ResourceLoader::instance().load<gkit::resource::ShaderSource>(path);

    gkit::test::assert_if(bool(result), "load<ShaderSource> must succeed");
    const auto& shader = result.resource;
    gkit::test::assert_if(shader->is_loaded(), "shader must be loaded");

    auto full = shader->source();
    gkit::test::assert_if(full.find("#shader vertex") != std::string_view::npos, "source must contain vertex marker");
    gkit::test::assert_if(full.find("#shader fragment") != std::string_view::npos,
                          "source must contain fragment marker");
    gkit::test::logln("  raw source:\n{}", full);

    gkit::test::assert_if(shader->has_stage(gkit::resource::ShaderStage::Vertex), "vertex stage must exist");
    gkit::test::assert_if(shader->has_stage(gkit::resource::ShaderStage::Fragment), "fragment stage must exist");

    auto vs = shader->get_source(gkit::resource::ShaderStage::Vertex);
    gkit::test::assert_if(vs.find("gl_Position") != std::string_view::npos, "vertex source must contain gl_Position");
    gkit::test::assert_if(vs.find("aPos") != std::string_view::npos, "vertex source must contain aPos");
    gkit::test::logln("  vertex shader:\n{}", vs);

    auto fs = shader->get_source(gkit::resource::ShaderStage::Fragment);
    gkit::test::assert_if(fs.find("FragColor") != std::string_view::npos, "fragment source must contain FragColor");
    gkit::test::logln("  fragment shader:\n{}", fs);

    gkit::test::logln("  vertex and fragment stages parsed correctly");
    return true;
}


auto test_cache_returns_same_instance() -> bool {
    gkit::test::logln("=== Test: Cache Returns Same Instance ===");

    const auto path = resource_dir() / "basic.shader";

    auto first  = gkit::resource::ResourceLoader::instance().load<gkit::resource::ShaderSource>(path);
    auto second = gkit::resource::ResourceLoader::instance().load<gkit::resource::ShaderSource>(path);

    gkit::test::assert_if(bool(first), "first load must succeed");
    gkit::test::assert_if(bool(second), "second load must succeed");
    gkit::test::assert_if(first.resource.get() == second.resource.get(),
                          "loading same path twice must return cached instance");

    gkit::test::logln("  loading same path twice returns cached instance");
    return true;
}


auto test_load_missing_file() -> bool {
    gkit::test::logln("=== Test: Load Missing File ===");

    auto result = gkit::resource::ResourceLoader::instance().load<gkit::resource::ShaderSource>(
        resource_dir() / "does_not_exist.shader");

    gkit::test::assert_if(!bool(result), "loading missing file must report failure");
    gkit::test::logln("  missing file reports failure");
    return true;
}


auto main() -> int {
    gkit::test::logln("========================================");
    gkit::test::logln("   gkit::resource::ShaderSource Tests   ");
    gkit::test::logln("========================================");

    auto test_runner = gkit::test::TestRunner()
                           .add_test_func(test_load_vertex_and_fragment)
                           .add_test_func(test_cache_returns_same_instance)
                           .add_test_func(test_load_missing_file);

    test_runner.run();
    return 0;
}
