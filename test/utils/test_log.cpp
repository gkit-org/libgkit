#include <gkit/gkit.hpp>
#include <iostream>
#include <thread>
#include <vector>

using gkit::utils::Log;

auto main() -> int {
    auto& logger = Log::instance();
    logger.set_log_file_path("./test_log.txt");

    constexpr int producer_count = 8;
    constexpr int logs_per_thread = 50000;
    std::vector<std::thread> producers;
    producers.reserve(producer_count);

    for (int thread_id = 0; thread_id < producer_count; ++thread_id) {
        producers.emplace_back([thread_id, &logger]() {
            for (int i = 0; i < logs_per_thread; ++i) {
                logger.log({
                    .message = "T" + std::to_string(thread_id) + " log " + std::to_string(i),
                    .level = static_cast<Log::LogLevel>(i % 3),
                    .functions = static_cast<std::uint8_t>(Log::LogFunction::Both)
                });
            }
        });
    }

    for (auto& producer : producers) {
        producer.join();
    }

    logger.flush();

    const auto s = logger.stats();
    const std::uint64_t total = static_cast<std::uint64_t>(producer_count) * static_cast<std::uint64_t>(logs_per_thread);

    std::cout << "total=" << total
              << " enqueued=" << s.enqueued
              << " dropped=" << s.dropped_full
              << " processed=" << s.processed
              << std::endl;

    if (s.enqueued + s.dropped_full != total) {
        std::cerr << "stats mismatch" << std::endl;
        return 1;
    }

    std::cout << "Finished logging" << std::endl;
    return 0;
}
