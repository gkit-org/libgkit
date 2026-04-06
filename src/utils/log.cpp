#include "gkit/utils/log.hpp"
#include <mutex>
#include <cstdio>

static constexpr const char* INFO_LOG_TIP    = "[INFO]";
static constexpr const char* WARNING_LOG_TIP = "[WARNING]";
static constexpr const char* ERROR_LOG_TIP   = "[ERROR]";


gkit::utils::Log::Log() : log_msg_queue(), log_msg_queue_mutex(), log_thread_cv(), log_thread(&Log::log_handler, this) {

}


gkit::utils::Log::~Log() {
    this->log_enable.store(false);
    log_thread_cv.notify_all();
    if (log_thread.joinable()) {
        log_thread.join();
    }
}


auto gkit::utils::Log::log(Message msg) -> void {
    std::lock_guard<std::mutex> lck(log_msg_queue_mutex);
    log_msg_queue.push(msg);
    log_thread_cv.notify_all();
}


auto gkit::utils::Log::log_to_console(const std::string& msg, LogLevel level) -> void {
    char* log_tip = nullptr;
    if (level == LogLevel::Info) {
        log_tip = const_cast<char*>(INFO_LOG_TIP);
    } else if (level == LogLevel::Warning) {
        log_tip = const_cast<char*>(WARNING_LOG_TIP);
    } else if (level == LogLevel::Error) {
        log_tip = const_cast<char*>(ERROR_LOG_TIP);
    }

    std::lock_guard<std::mutex> lck(console_log_mutex);
    std::printf("%s %s\n", log_tip, msg.c_str());    
}


auto gkit::utils::Log::log_to_file(const std::string& msg, LogLevel level) -> void {
    // Todo
}


auto gkit::utils::Log::log_handler() -> void {
    while (this->log_enable.load()) {
        std::unique_lock<std::mutex> q_lck(log_msg_queue_mutex);
        log_thread_cv.wait(q_lck, [this]() { return !log_msg_queue.empty() || !log_enable.load(); });

        while(!log_msg_queue.empty()) {
            auto msg = log_msg_queue.front();
            log_msg_queue.pop();
            
            if (msg.functions & static_cast<std::uint8_t>(LogFunction::Console)) {
                log_to_console(msg.message, msg.level);
            } else if (msg.functions & static_cast<std::uint8_t>(LogFunction::File)) {
                log_to_file(msg.message, msg.level);
            } else if (msg.functions & static_cast<std::uint8_t>(LogFunction::Both)) {
                log_to_console(msg.message, msg.level);
                log_to_file(msg.message, msg.level);
            }
        }
    }
}
