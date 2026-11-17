#include "gkit/resource/resource_loader.hpp"
#include "gkit/resource/shader_source.hpp"
#include "gkit/resource/texture.hpp"
#include "test_utils.hpp"

#include <barrier>
#include <fstream>
#include <thread>

namespace {
    struct Counting : gkit::resource::Resource {
        using Resource::Resource;
        static inline std::atomic<int> loads = 0;
        auto load_from_file() -> void override {
            ++loads;
            this->is_available = true;
        }
    };
    auto test_cache() -> bool {
        using namespace gkit::resource;
        auto path    = std::filesystem::path(__FILE__).parent_path() / "basic.shader";
        auto& loader = ResourceLoader::instance();
        loader.clear();
        std::barrier start(8);
        std::vector<std::shared_ptr<const Counting>> results(8);
        std::vector<std::thread> threads;
        for (int i = 0; i < 8; ++i)
            threads.emplace_back([&, i] {
                start.arrive_and_wait();
                results[i] = loader.load_result<Counting>(path).resource;
            });
        for (auto& thread : threads) thread.join();
        for (const auto& result : results) gkit::test::assert_if(result == results[0], "concurrent cache identity");
        gkit::test::assert_if(Counting::loads == 1, "load once");
        auto alias = loader.load_result<Counting>(path.parent_path() / "." / path.filename());
        gkit::test::assert_if(alias.resource == results[0], "normalized path identity");
        gkit::test::assert_if(bool(loader.load_result<ShaderSource>(path)), "cache distinguishes resource type");
        std::weak_ptr<const Counting> weak = results[0];
        results.clear();
        alias.resource.reset();
        gkit::test::assert_if(weak.expired(), "cache must not retain unused resources");
        gkit::test::assert_if(!loader.load_result<ShaderSource>(path.string() + ".missing"), "missing file failure");
        gkit::test::assert_if(!loader.load_result<Texture>(path), "shader is not an image");
        return true;
    }
    auto test_retry_and_format() -> bool {
        using namespace gkit::resource;
        auto path = std::filesystem::temp_directory_path() /
                    ("gkit-shader-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        struct Cleanup {
            std::filesystem::path path;
            ~Cleanup() {
                std::error_code error;
                std::filesystem::remove(path, error);
            }
        } cleanup{path};
        auto& loader = ResourceLoader::instance();
        auto missing = loader.load_result<ShaderSource>(path);
        gkit::test::assert_if(missing.error == LoadError::Read, "missing file has a read error");
        {
            std::ofstream file(path);
            file << "#shader vertex\n#shader fragment\nvoid main() {}\n";
        }
        gkit::test::assert_if(loader.load_result<ShaderSource>(path).error == LoadError::Decode,
                              "empty shader stage must fail");
        {
            std::ofstream file(path);
            file << "  #shader vertex  \r\nvoid main() {}\r\n#shader fragment\r\nvoid main() {}\r\n";
        }
        auto loaded = loader.load_result<ShaderSource>(path);
        gkit::test::assert_if(bool(loaded) && loaded.resource->has_stage(ShaderStage::Vertex),
                              "failed loads are retryable and CRLF markers parse");
        loader.clear();
        gkit::test::assert_if(loaded.resource->is_loaded(), "clear preserves existing handles");
        {
            std::ofstream file(path);
            file << "#shader unknown\ninvalid\n";
        }
        gkit::test::assert_if(!loader.load_result<ShaderSource>(path), "unknown marker rejected");
        return true;
    }
    auto test_images() -> bool {
        auto root = std::filesystem::path(__FILE__).parent_path().parent_path().parent_path();
        auto image =
            gkit::resource::ResourceLoader::instance().load_result<gkit::resource::Texture>(root / "gkit_logo.png");
        gkit::test::assert_if(bool(image), "decode repository PNG without graphics context");
        gkit::test::assert_if(image.resource->width() > 0 && image.resource->height() > 0, "image dimensions");
        gkit::test::assert_if(image.resource->pixels().size() ==
                                  static_cast<std::size_t>(image.resource->pitch()) * image.resource->height(),
                              "image byte span");
        return true;
    }
} // namespace
auto main() -> int {
    gkit::test::TestRunner()
        .add_test_func(test_cache)
        .add_test_func(test_images)
        .add_test_func(test_retry_and_format)
        .run();
}
