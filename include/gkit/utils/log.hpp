#pragma once

#include "gkit/core/scene/singleton.hpp"
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace gkit::utils {
    class Log : public gkit::scene::Singleton<Log> {
        friend class gkit::scene::Singleton<Log>;
    public:
        enum class LogLevel : std::uint8_t {
            Info,
            Warning,
            Error
        }; // enum class LogLevel

        enum class LogFunction : std::uint8_t {
            Console = 1u,
            File = 1u << 1u,
            Both = Console | File
        }; // enum class LogFunction

        struct Message {
            std::string message;
            LogLevel level;
            std::uint8_t functions;
        }; // struct Message

    private:
        Log();
        ~Log();

        auto log_handler() -> void;
        auto log_to_console(const std::string& msg, LogLevel level = LogLevel::Info) -> void;
        auto log_to_file(const std::string& msg, LogLevel level = LogLevel::Info) -> void;
    public:
        /**
         * @brief Log a message to the log system. The message will be processed by the log thread and output to the console or file according to the log function specified in the message.
         * @param msg The log message to be logged.
         */
        auto log(Message msg) -> void;

    private:
        std::atomic<bool> log_enable {true};

        std::filesystem::path log_file_path = "~/gkit/log.txt";
        std::mutex log_msg_queue_mutex;
        std::queue<Message> log_msg_queue;

        std::thread log_thread;
        std::condition_variable log_thread_cv;

        std::mutex console_log_mutex {};
        std::mutex file_log_mutex {};
    }; // class Log
} // namespace gkit::utils