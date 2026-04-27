#include "gkit/utils/log.hpp"
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <thread>

static constexpr const char* INFO_LOG_TIP    = "[INFO]";
static constexpr const char* WARNING_LOG_TIP = "[WARNING]";
static constexpr const char* ERROR_LOG_TIP   = "[ERROR]";

namespace {
    /** @brief Keep log level prefix formatting consistent across console and file sinks. */
    auto level_tip(gkit::utils::Log::LogLevel level) -> const char* {
        if (level == gkit::utils::Log::LogLevel::Info) {
            return INFO_LOG_TIP;
        }
        if (level == gkit::utils::Log::LogLevel::Warning) {
            return WARNING_LOG_TIP;
        }
        return ERROR_LOG_TIP;
    }

    /** @brief Expand leading "~/" to HOME so default path works cross-environment. */
    auto resolve_log_path(const std::filesystem::path& configured_path) -> std::filesystem::path {
        const auto configured = configured_path.string();
        if (configured.rfind("~/", 0u) == 0u) {
            if (const char* home = std::getenv("HOME")) {
                return std::filesystem::path(home) / configured.substr(2);
            }
        }

        return configured_path;
    }
} // namespace

gkit::utils::Log::MpscBoundedQueue::MpscBoundedQueue(std::size_t requested_capacity)
    : capacity(1),
      mask(0),
      buffer(),
      enqueue_pos(0),
      dequeue_pos(0) 
{
    // Normalize to power-of-two so (index & mask) is equivalent to modulo.
    while (capacity < requested_capacity) {
        capacity <<= 1u;
    }
    mask = capacity - 1u;

    buffer = std::make_unique<Slot[]>(capacity);
    for (std::size_t i = 0; i < capacity; ++i) {
        buffer[i].sequence.store(i, std::memory_order_relaxed);
    }
}

auto gkit::utils::Log::MpscBoundedQueue::try_enqueue(Message&& msg) noexcept -> bool {
    std::size_t pos = enqueue_pos.load(std::memory_order_relaxed);
    for (;;) {
        auto& slot = buffer[pos & mask];
        const std::size_t seq = slot.sequence.load(std::memory_order_acquire);
        // diff == 0: slot is available for this producer position.
        // diff < 0: queue is full from this producer's view.
        const std::intptr_t diff = static_cast<std::intptr_t>(seq) - static_cast<std::intptr_t>(pos);

        if (diff == 0) {
            if (enqueue_pos.compare_exchange_weak(pos, pos + 1u, std::memory_order_relaxed)) {
                slot.message = std::move(msg);
                slot.sequence.store(pos + 1u, std::memory_order_release);
                return true;
            }
        } else if (diff < 0) {
            return false;
        } else {
            pos = enqueue_pos.load(std::memory_order_relaxed);
        }
    }
}

auto gkit::utils::Log::MpscBoundedQueue::try_dequeue(Message& msg) noexcept -> bool {
    const std::size_t pos = dequeue_pos.load(std::memory_order_relaxed);
    auto& slot = buffer[pos & mask];
    const std::size_t seq = slot.sequence.load(std::memory_order_acquire);
    const std::intptr_t diff = static_cast<std::intptr_t>(seq) - static_cast<std::intptr_t>(pos + 1u);

    if (diff == 0) {
        // Single consumer owns dequeue_pos, so store is sufficient here.
        dequeue_pos.store(pos + 1u, std::memory_order_relaxed);
        msg = std::move(slot.message);
        // Release this slot back to producers in next ring cycle.
        slot.sequence.store(pos + capacity, std::memory_order_release);
        return true;
    }

    return false;
}

auto gkit::utils::Log::MpscBoundedQueue::empty() const noexcept -> bool {
    const std::size_t pos = dequeue_pos.load(std::memory_order_relaxed);
    const auto& slot = buffer[pos & mask];
    const std::size_t seq = slot.sequence.load(std::memory_order_acquire);
    return static_cast<std::intptr_t>(seq) - static_cast<std::intptr_t>(pos + 1u) < 0;
}


gkit::utils::Log::Log() : log_msg_queue(LOG_QUEUE_CAPACITY), log_thread(&Log::log_handler, this) {}


gkit::utils::Log::~Log() {
    this->log_enable.store(false, std::memory_order_release);
    log_thread_cv.notify_all();

    if (log_thread.joinable()) {
        log_thread.join();
    }
}


auto gkit::utils::Log::log(Message msg) -> void {
    (void)try_log(std::move(msg));
}


auto gkit::utils::Log::try_log(Message&& msg) noexcept -> bool {
    if (!this->log_enable.load(std::memory_order_acquire)) {
        return false;
    }

    if (log_msg_queue.try_enqueue(std::move(msg))) {
        enqueued_count.fetch_add(1u, std::memory_order_relaxed);
        // Wake consumer, producer never blocks on this path.
        log_thread_cv.notify_one();
        return true;
    }

    dropped_full_count.fetch_add(1u, std::memory_order_relaxed);
    return false;
}


auto gkit::utils::Log::flush(std::chrono::milliseconds timeout) -> void {
    // Best-effort drain wait for shutdown/testing; no producer synchronization.
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (log_msg_queue.empty()) {
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}


auto gkit::utils::Log::stats() const noexcept -> Stats {
    return {
        .enqueued = enqueued_count.load(std::memory_order_relaxed),
        .dropped_full = dropped_full_count.load(std::memory_order_relaxed),
        .processed = processed_count.load(std::memory_order_relaxed)
    };
}


auto gkit::utils::Log::set_log_file_path(const std::filesystem::path& path) -> bool {
    if (path.empty()) {
        return false;
    }

    std::lock_guard<std::mutex> lck(file_log_mutex);
    log_file_path = path;

    if (log_file_stream.is_open()) {
        log_file_stream.close();
    }

    return true;
}


auto gkit::utils::Log::log_to_console(const std::string& msg, LogLevel level) -> void {
    const char* log_tip = level_tip(level);
    std::lock_guard<std::mutex> lck(console_log_mutex);
    std::printf("%s %s\n", log_tip, msg.c_str());
}


auto gkit::utils::Log::log_to_file(const std::string& msg, LogLevel level) -> void {
    std::lock_guard<std::mutex> lck(file_log_mutex);

    if (!log_file_stream.is_open()) {
        // Lazily open file sink on first use to avoid startup IO on logging-disabled paths.
        const auto resolved_path = resolve_log_path(log_file_path);

        std::error_code ec;
        const auto parent = resolved_path.parent_path();
        if (!parent.empty()) {
            std::filesystem::create_directories(parent, ec);
        }

        // Open in append mode so logs survive restarts and preserve historical context.
        log_file_stream.open(resolved_path, std::ios::out | std::ios::app);
        if (!log_file_stream.is_open()) {
            return;
        }
    }

    // Flush each record in MVP for correctness-first behavior.
    log_file_stream << level_tip(level) << ' ' << msg << '\n';
    log_file_stream.flush();
}


auto gkit::utils::Log::log_handler() -> void {
    Message msg;
    while (this->log_enable.load(std::memory_order_acquire) || !log_msg_queue.empty()) {
        bool consumed = false;

        while (log_msg_queue.try_dequeue(msg)) {
            consumed = true;
            processed_count.fetch_add(1u, std::memory_order_relaxed);

            if (msg.functions & static_cast<std::uint8_t>(LogFunction::Console)) {
                log_to_console(msg.message, msg.level);
            }
            if (msg.functions & static_cast<std::uint8_t>(LogFunction::File)) {
                log_to_file(msg.message, msg.level);
            }
        }

        if (!this->log_enable.load(std::memory_order_acquire) && log_msg_queue.empty()) {
            break;
        }

        if (!consumed) {
            std::unique_lock<std::mutex> wait_lock(log_thread_wait_mutex);
            // Timed wait avoids missed wakeups from racing notify calls.
            log_thread_cv.wait_for(wait_lock, LOG_WAIT_TIMEOUT);
        }
    }
}
