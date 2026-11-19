#include "gkit/core/log.hpp"
#include "test_utils.hpp"

#include <fstream>
#include <thread>

auto test_flush() -> bool {
    auto& log = gkit::core::Log::instance();
    const auto path =
        std::filesystem::temp_directory_path() /
        ("gkit-flush-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".txt");
    gkit::test::assert_if(log.set_log_file_path(path), "set output path");
    std::vector<std::thread> producers;
    for (int i = 0; i < 4; ++i)
        producers.emplace_back([&]() -> void {
            for (int n = 0; n < 100; ++n) {
                log.log({"flush-record",
                         gkit::core::Log::LogLevel::Info,
                         static_cast<std::uint8_t>(gkit::core::Log::LogFunction::File)});
            }
        });
    for (auto& thread : producers) thread.join();
    gkit::test::assert_if(log.flush(std::chrono::seconds(5)), "flush must wait for sink completion");
    std::ifstream file(path);
    std::string line;
    int count = 0;
    while (std::getline(file, line)) ++count;
    gkit::test::assert_if(count == 400, "all accepted records must be visible after flush");
    log.shutdown();
    gkit::test::assert_if(!log.try_log({"rejected", gkit::core::Log::LogLevel::Info, 0}), "shutdown rejects producers");
    file.close();
    log.set_log_file_path(path.string() + ".closed"); // Close sink before removing on Windows.
    std::filesystem::remove(path);
    return true;
}


auto main() -> int {
    gkit::test::TestRunner().add_test_func(test_flush).run();
}
