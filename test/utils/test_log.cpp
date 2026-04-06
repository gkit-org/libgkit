#include <gkit/gkit.hpp>
#include <iostream>

using gkit::utils::Log;

auto main() -> int {
    auto& logger = Log::instance();

    for (int i = 0; i < 100000; ++i) {
        Log::Message msg = {
            .message = "Log message" + std::to_string(i),
            .level = static_cast<Log::LogLevel>(i % 3),
            .functions = static_cast<std::uint8_t>(Log::LogFunction::Console)
        };

        logger.log(msg);
    }

    std::cout << "Finished logging" << std::endl;
    return 0;
}