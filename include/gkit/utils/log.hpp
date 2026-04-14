#pragma once

#include "gkit/core/scene/singleton.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace gkit::utils {
    class Log : public gkit::core::scene::Singleton<Log> {
        friend class gkit::core::scene::Singleton<Log>;
    public:
        enum class LogLevel : std::uint8_t {
            Info,
            Warning,
            Error
        }; // enum class LogLevel

        enum class LogFunction : std::uint8_t {
            Console = 1u,
            File    = 1u << 1u,
            Both    = Console | File
        }; // enum class LogFunction

        struct Message {
            std::string message;
            LogLevel level;
            std::uint8_t functions;
        }; // struct Message

        struct Stats {
            std::uint64_t enqueued;
            std::uint64_t dropped_full;
            std::uint64_t processed;
        }; // struct Stats

    private:
        Log();
        ~Log();

        auto log_handler() -> void;
        auto log_to_console(const std::string& msg, LogLevel level = LogLevel::Info) -> void;
        auto log_to_file(const std::string& msg, LogLevel level = LogLevel::Info) -> void;

        class MpscBoundedQueue {
        public:
            explicit MpscBoundedQueue(std::size_t capacity);
            // Non-blocking producer path. Returns false when queue is full.
            auto try_enqueue(Message&& msg) noexcept -> bool;
            // Single-consumer dequeue path.
            auto try_dequeue(Message& msg) noexcept -> bool;
            auto empty() const noexcept -> bool;

        private:
            struct Slot {
                std::atomic<std::size_t> sequence;
                Message message;

                Slot() : sequence(0), message() {}
            }; // struct Slot

            std::size_t capacity;
            std::size_t mask;
            std::unique_ptr<Slot[]> buffer;
            std::atomic<std::size_t> enqueue_pos;
            std::atomic<std::size_t> dequeue_pos;
        }; // class MpscBoundedQueue

    public:
        /**
         * @brief Log a message to the log system. The message will be processed by the log thread and output to the console or file according to the log function specified in the message.
         * @param msg The log message to be logged.
         */
        auto log(Message msg) -> void;
        /** @brief Exposed for low-level call sites that want enqueue success feedback. */
        auto try_log(Message&& msg) noexcept -> bool;
        /** @brief Best-effort wait for queue drain. */
        auto flush(std::chrono::milliseconds timeout = std::chrono::milliseconds(100)) -> void;
        auto stats() const noexcept -> Stats;

    private:
        // Must be power-of-two effective capacity for ring index masking.
        static constexpr std::size_t LOG_QUEUE_CAPACITY = 8192;
        static_assert((LOG_QUEUE_CAPACITY & (LOG_QUEUE_CAPACITY - 1u)) == 0u,
                      "LOG_QUEUE_CAPACITY must be a power of two");
        static constexpr std::chrono::milliseconds LOG_WAIT_TIMEOUT = std::chrono::milliseconds(2);

        std::atomic<bool> log_enable {true};

        std::filesystem::path log_file_path = "~/gkit/log.txt";
        MpscBoundedQueue log_msg_queue;

        std::thread log_thread;
        std::condition_variable log_thread_cv;
        std::mutex log_thread_wait_mutex;

        std::mutex console_log_mutex {};
        std::mutex file_log_mutex {};
        std::ofstream log_file_stream {};

        std::atomic<std::uint64_t> enqueued_count {0};
        std::atomic<std::uint64_t> dropped_full_count {0};
        std::atomic<std::uint64_t> processed_count {0};
    }; // class Log
} // namespace gkit::utils
