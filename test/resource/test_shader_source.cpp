#include <cassert>
#include <filesystem>
#include <iostream>
#include <string_view>

#include <gkit/resource/resource_manager.hpp>
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

auto test_load_and_get_by_id() -> void {
    std::cout << "=== Test: Load and Get by ID ===" << '\n';

    const auto path = resource_dir() / "basic.shader";
    auto id_opt     = gkit::resource::ResourceManager::instance().load<gkit::resource::ShaderSource>(path);

    assert(id_opt.has_value());
    auto id = id_opt.value();

    // Retrieve by ID
    auto result = gkit::resource::ResourceManager::instance().get<gkit::resource::ShaderSource>(id);
    assert(result.has_value());
    const auto& shader = result.value();
    assert(shader->is_loaded());
    assert(shader->get_id() == id);

    // Print raw file content
    auto full = shader->source();
    std::cout << "\n--- Raw Shader Source ---\n" << full << "--- End Raw Source ---\n" << '\n';

    // Print per-stage sources
    std::cout << "--- Vertex Shader ---\n"
              << shader->get_source(gkit::resource::ShaderStage::Vertex) << "--- End Vertex Shader ---\n"
              << '\n';
    std::cout << "--- Fragment Shader ---\n"
              << shader->get_source(gkit::resource::ShaderStage::Fragment) << "--- End Fragment Shader ---\n"
              << '\n';

    assert(full.find("#shader vertex") != std::string_view::npos);
    assert(full.find("#shader fragment") != std::string_view::npos);

    // Per-stage access
    assert(shader->has_stage(gkit::resource::ShaderStage::Vertex));
    assert(shader->has_stage(gkit::resource::ShaderStage::Fragment));

    auto vs = shader->get_source(gkit::resource::ShaderStage::Vertex);
    assert(vs.find("gl_Position") != std::string_view::npos);
    assert(vs.find("aPos") != std::string_view::npos);

    auto fs = shader->get_source(gkit::resource::ShaderStage::Fragment);
    assert(fs.find("FragColor") != std::string_view::npos);

    std::cout << "  Vertex and fragment stages parsed correctly: OK" << '\n';
    std::cout << "Test passed!" << '\n' << '\n';
}

auto test_load_twice_returns_same_id() -> void {
    std::cout << "=== Test: Load Twice Returns Same ID ===" << '\n';

    const auto path = resource_dir() / "basic.shader";

    auto first  = gkit::resource::ResourceManager::instance().load<gkit::resource::ShaderSource>(path);
    auto second = gkit::resource::ResourceManager::instance().load<gkit::resource::ShaderSource>(path);

    assert(first.has_value());
    assert(second.has_value());
    assert(first.value() == second.value());

    // Retrieve by ID — should get same instance
    auto shader1 = gkit::resource::ResourceManager::instance().get<gkit::resource::ShaderSource>(first.value());
    auto shader2 = gkit::resource::ResourceManager::instance().get<gkit::resource::ShaderSource>(second.value());

    assert(shader1.has_value());
    assert(shader2.has_value());
    assert(shader1.value().get() == shader2.value().get());

    std::cout << "  Same path -> same ID -> same cached instance: OK" << '\n';
    std::cout << "Test passed!" << '\n' << '\n';
}

auto test_get_nonexistent_id() -> void {
    std::cout << "=== Test: Get Nonexistent ID ===" << '\n';

    auto id     = gkit::resource::ResourceId::from_string("never_loaded");
    auto result = gkit::resource::ResourceManager::instance().get<gkit::resource::ShaderSource>(id);

    assert(!result.has_value());

    std::cout << "  Nonexistent ID returns nullopt: OK" << '\n';
    std::cout << "Test passed!" << '\n' << '\n';
}

auto test_load_missing_file() -> void {
    std::cout << "=== Test: Load Missing File ===" << '\n';

    auto id_opt = gkit::resource::ResourceManager::instance().load<gkit::resource::ShaderSource>(
        resource_dir() / "does_not_exist.shader");

    assert(!id_opt.has_value());

    std::cout << "  Missing file returns nullopt: OK" << '\n';
    std::cout << "Test passed!" << '\n' << '\n';
}

auto main() -> int {
    std::cout << "========================================" << '\n';
    std::cout << "   gkit::resource::ShaderSource Tests   " << '\n';
    std::cout << "========================================" << '\n' << '\n';

    test_load_and_get_by_id();
    test_load_twice_returns_same_id();
    test_get_nonexistent_id();
    test_load_missing_file();

    std::cout << "========================================" << '\n';
    std::cout << "         ALL TESTS PASSED!             " << '\n';
    std::cout << "========================================" << '\n';

    return 0;
}
