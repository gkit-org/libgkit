#include "test_utils.hpp"

#include <cstdint>
#include <string>
#include <thread>
#include <vector>

#include <gkit/core/log.hpp>

using gkit::core::Log;

auto test_concurrent_logging() -> bool {
    gkit::test::logln("=== concurrent logging ===");

    auto& logger = Log::instance();
    logger.set_log_file_path("./test_log.txt");

    constexpr int producer_count  = 8;
    constexpr int logs_per_thread = 50000;
    std::vector<std::thread> producers;
    producers.reserve(producer_count);

    for (int thread_id = 0; thread_id < producer_count; ++thread_id) {
        producers.emplace_back([thread_id, &logger]() -> void {
            for (int i = 0; i < logs_per_thread; ++i) {
                logger.log({.message   = "T" + std::to_string(thread_id) + " log " + std::to_string(i),
                            .level     = static_cast<Log::LogLevel>(i % 3),
                            .functions = static_cast<std::uint8_t>(Log::LogFunction::Both)});
            }
        });
    }

    for (auto& producer : producers) {
        producer.join();
    }

    logger.flush();

    const auto s = logger.stats();
    const std::uint64_t total =
        static_cast<std::uint64_t>(producer_count) * static_cast<std::uint64_t>(logs_per_thread);

    gkit::test::logln("  total={} enqueued={} dropped={} processed={}", total, s.enqueued, s.dropped_full, s.processed);

    gkit::test::assert_if(s.enqueued + s.dropped_full == total, "stats mismatch: enqueued + dropped != total");
    gkit::test::logln("  Finished logging");
    return true;
}


auto main() -> int {
    auto test_runner = gkit::test::TestRunner().add_test_func(test_concurrent_logging);

    test_runner.run();
    return 0;
}
