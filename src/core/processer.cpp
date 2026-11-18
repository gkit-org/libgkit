/** @file processer.cpp
 * @brief Persistent worker scheduling, frame barriers and exception-safe run cleanup.
 */
#include "gkit/core/processer.hpp"

#include "core/node_internal.hpp"
#include "gkit/core/input/input.hpp"

#include <algorithm>
#include <cmath>
#include <deque>
#include <limits>
#include <thread>

namespace gkit {
    namespace {
        /**
         * @brief Persistent workers with one barrier per update phase and first-exception capture.
         * @note No worker recursively submits/waits for child tasks. Failed phases skip queued jobs and join
         * in-flight callbacks before propagating failure. Partial construction joins already started threads.
         */
        class WorkerPool {
            std::mutex mutex;
            std::condition_variable available, finished;
            std::deque<std::function<void()>> jobs;
            std::vector<std::thread> workers;
            std::size_t pending = 0;
            bool closing        = false;
            std::exception_ptr failure;

        public:
            auto enabled() const noexcept -> bool { return !workers.empty(); }
            explicit WorkerPool(std::size_t count) {
                try {
                    for (std::size_t i = 0; i < count; ++i)
                        workers.emplace_back([this] {
                            for (;;) {
                                std::function<void()> job;
                                bool skip;
                                {
                                    std::unique_lock lock(mutex);
                                    available.wait(lock, [&] {
                                        return closing || !jobs.empty();
                                    });
                                    if (jobs.empty()) return;
                                    job = std::move(jobs.front());
                                    jobs.pop_front();
                                    skip = bool(failure);
                                }
                                try {
                                    if (!skip) job();
                                } catch (...) {
                                    std::lock_guard lock(mutex);
                                    if (!failure) failure = std::current_exception();
                                }
                                {
                                    std::lock_guard lock(mutex);
                                    if (--pending == 0) finished.notify_all();
                                }
                            }
                        });
                } catch (...) {
                    close();
                    throw;
                }
            }
            ~WorkerPool() { close(); }
            auto close() -> void {
                {
                    std::lock_guard lock(mutex);
                    closing = true;
                }
                available.notify_all();
                for (auto& worker : workers)
                    if (worker.joinable()) worker.join();
            }
            auto check_failure() -> void {
                std::lock_guard lock(mutex);
                if (failure) std::rethrow_exception(failure);
            }
            auto cancel(std::exception_ptr error) -> void {
                std::lock_guard lock(mutex);
                if (!failure) failure = error;
            }
            auto submit(std::function<void()> job) -> void {
                std::lock_guard lock(mutex);
                if (failure) std::rethrow_exception(failure);
                jobs.push_back(std::move(job));
                ++pending;
                available.notify_one();
            }
            auto join() -> void {
                std::unique_lock lock(mutex);
                finished.wait(lock, [&] {
                    return pending == 0;
                });
                if (failure) std::rethrow_exception(failure);
            }
        };
        using State = core::detail::NodeState;
        /**
         * @brief Walk one worker-owned subtree parent-first, applying inherited update gates.
         * @note User code may post work or stage edits, but cannot mutate topology during a phase.
         */
        auto serial_update(State* root, double delta, bool physics) -> void {
            std::vector<State*> pending{root};
            while (!pending.empty()) {
                auto* current = pending.back();
                pending.pop_back();
                auto& node = current->node;
                if (!node.updates_enabled()) continue;
                if (physics)
                    node.physics_process(delta);
                else
                    node.process(delta);
                if (!node.updates_enabled()) continue;
                for (auto* child : current->descendants() | std::views::reverse) pending.push_back(child);
            }
        }
        /**
         * @brief Update coordinator nodes and dispatch each independent subtree after its ancestors have completed.
         * @note Always join the phase before returning or throwing. No ordering is promised between independent
         * domains; local traversal remains insertion-ordered. This barrier separates every physics and logic phase.
         */
        auto update(State* root, double delta, bool physics, WorkerPool& pool) -> void {
            std::exception_ptr failure;
            try {
                std::vector<State*> pending{root};
                while (!pending.empty()) {
                    pool.check_failure();
                    auto* current = pending.back();
                    pending.pop_back();
                    if (pool.enabled() && current != root && current->node.is_parallel()) {
                        pool.submit([current, delta, physics] {
                            serial_update(current, delta, physics);
                        });
                        continue;
                    }
                    auto& node = current->node;
                    if (!node.updates_enabled()) continue;
                    if (physics)
                        node.physics_process(delta);
                    else
                        node.process(delta);
                    if (!node.updates_enabled()) continue;
                    for (auto* child : current->descendants() | std::views::reverse) pending.push_back(child);
                }
            } catch (...) {
                failure = std::current_exception();
                pool.cancel(failure);
            }
            try {
                pool.join();
            } catch (...) {
                if (!failure) failure = std::current_exception();
            }
            if (failure) std::rethrow_exception(failure);
        }
    } // namespace
    Processer::Processer() :
        root_owner(core::UniqueObject::create<core::Node>()), runtime(std::make_unique<core::detail::TreeRuntime>()) {
        std::lock_guard lock(core::detail::tree_mutex);
        core::detail::assign_runtime(root().state.get(), runtime.get());
    }
    Processer::~Processer() {
        // Destruction must follow run() returning; stop() is the cross-thread operation.
        {
            std::lock_guard lock(runtime->queue_mutex);
            runtime->accepting = false;
        }
        try {
            core::detail::drain_edits(*runtime, true);
        } catch (...) {
        }
        try {
            core::detail::drain_posts(*runtime, true);
        } catch (...) {
        }
        std::lock_guard guard(core::detail::tree_mutex);
        core::detail::assign_runtime(root().state.get(), nullptr);
    }
    auto Processer::root() noexcept -> core::Node& {
        return *static_cast<core::Node*>(root_owner.get());
    }
    auto Processer::root() const noexcept -> const core::Node& {
        return *static_cast<const core::Node*>(root_owner.get());
    }
    auto Processer::set_parallel_update_enabled(bool value) noexcept -> Processer& {
        std::lock_guard lock(settings_mutex);
        config.parallel_update_enabled = value;
        return *this;
    }
    auto Processer::set_worker_count(std::size_t value) noexcept -> Processer& {
        std::lock_guard lock(settings_mutex);
        config.worker_count = value;
        return *this;
    }
    auto Processer::set_fixed_delta(double value) noexcept -> Processer& {
        std::lock_guard lock(settings_mutex);
        config.fixed_delta = value;
        return *this;
    }
    auto Processer::set_max_fixed_steps(std::size_t value) noexcept -> Processer& {
        std::lock_guard lock(settings_mutex);
        config.max_fixed_steps = value;
        return *this;
    }
    auto Processer::set_max_frame_delta(double value) noexcept -> Processer& {
        std::lock_guard lock(settings_mutex);
        config.max_frame_delta = value;
        return *this;
    }
    auto Processer::set_target_frame_rate(double value) noexcept -> Processer& {
        std::lock_guard lock(settings_mutex);
        config.target_frame_rate = value;
        return *this;
    }
    auto Processer::set_platform_options(platform::RuntimeOptions value) -> Processer& {
        std::lock_guard lock(settings_mutex);
        config.platform_options = value;
        return *this;
    }
    auto Processer::set_window_options(std::optional<platform::WindowOptions> value) -> Processer& {
        std::lock_guard lock(settings_mutex);
        config.window_options = std::move(value);
        return *this;
    }
    auto Processer::window() -> platform::Window& {
        platform::require_main_thread();
        if (!main_window) throw std::logic_error("no active main window");
        return *main_window;
    }
    auto Processer::set_initialize_sdl(bool value) noexcept -> Processer& {
        std::lock_guard lock(settings_mutex);
        config.initialize_sdl = value;
        return *this;
    }
    auto Processer::set_sdl_flags(std::optional<std::uint32_t> value) noexcept -> Processer& {
        std::lock_guard lock(settings_mutex);
        config.sdl_flags = value;
        return *this;
    }
    auto Processer::now() const -> std::chrono::steady_clock::time_point {
        return std::chrono::steady_clock::now();
    }
    auto Processer::wait_until(std::chrono::steady_clock::time_point deadline) -> void {
        std::unique_lock lock(wait_mutex);
        wake.wait_until(lock, deadline, [&] {
            return !running.load();
        });
    }
    auto Processer::stop() noexcept -> void {
        {
            std::lock_guard lock(wait_mutex);
            running.store(false);
        }
        wake.notify_all();
    }
    auto Processer::get_frame_context() const -> core::FrameContext {
        std::lock_guard lock(frame_mutex);
        return frame_context;
    }
    /**
     * @brief Execute the scheduler's startup, frame loop and unconditional cleanup boundary.
     * @note Workers are scoped inside the guarded run body, so they finish before tree exit callbacks run.
     * Shutdown closes queues before callbacks, preserves the first exception, releases only acquired SDL subsystems,
     * and clears the re-entry guard only after cleanup. Remaining whole physics backlog is counted and discarded.
     */
    auto Processer::run() -> void {
        Config settings;
        {
            std::lock_guard lock(settings_mutex);
            if (in_run) throw std::logic_error("run is not reentrant");
            if (runtime->thread != std::this_thread::get_id()) throw std::logic_error("run on the creating thread");
            settings = config;
            if (!std::isfinite(settings.fixed_delta) || settings.fixed_delta <= 0 || settings.max_fixed_steps == 0 ||
                !std::isfinite(settings.max_frame_delta) || settings.max_frame_delta <= 0 ||
                !std::isfinite(settings.target_frame_rate) || settings.target_frame_rate < 0 ||
                (settings.parallel_update_enabled && settings.worker_count == 0) ||
                settings.max_frame_delta / settings.fixed_delta >=
                    static_cast<double>(std::numeric_limits<std::uint64_t>::max()) ||
                (settings.target_frame_rate > 0 &&
                 1.0 / settings.target_frame_rate >
                     std::chrono::duration<double>(std::chrono::steady_clock::duration::max()).count() / 2))
                throw std::invalid_argument("invalid frame settings");
            in_run = true;
            running.store(true);
        }
        dropped_steps.store(0);
        std::exception_ptr failure;
        std::optional<platform::Runtime> session;
        bool started = false;
        try {
            {
                std::lock_guard lock(frame_mutex);
                frame_context = {};
            }
            WorkerPool pool(settings.parallel_update_enabled ? settings.worker_count : 0);
            if (settings.platform_options)
                session.emplace(*settings.platform_options);
            else if (settings.initialize_sdl) {
                if (settings.sdl_flags)
                    session.emplace(*settings.sdl_flags);
                else
                    session.emplace(platform::RuntimeOptions{true, true, true, true, true});
            }
            if (settings.window_options) {
                if (!session || !session->has_video()) throw std::invalid_argument("main window requires video");
                main_window = std::make_unique<platform::Window>(*settings.window_options);
            }
            if (session && session->has_events()) Input::instance().initialize();
            start_run_services();
            {
                std::lock_guard lock(runtime->queue_mutex);
                runtime->accepting = true;
            }
            {
                std::lock_guard lock(core::detail::tree_mutex);
                runtime->active = true;
            }
            started = true;
            on_start();
            auto previous       = now();
            double accumulated  = 0;
            std::uint64_t frame = 0;
            while (running.load()) {
                const auto start = now();
                const auto delta =
                    std::clamp(std::chrono::duration<double>(start - previous).count(), 0.0, settings.max_frame_delta);
                previous = start;
                accumulated += delta;
                {
                    std::lock_guard lock(frame_mutex);
                    frame_context.delta = delta;
                    frame_context.frame = frame;
                }
                if (session && session->has_events()) {
                    auto events = session->poll_events();
                    for (const auto& event : events) {
                        if (std::holds_alternative<platform::QuitEvent>(event)) stop();
                        if (const auto* change = std::get_if<platform::WindowEvent>(&event);
                            change && main_window && change->window == main_window->state().id) {
                            if (change->change == platform::WindowChange::Close)
                                stop();
                            else
                                main_window->refresh();
                        }
                    }
                    Input::instance().update(events);
                }
                if (!running.load()) break;
                if (frame == 0) core::detail::enter_nodes(root().state.get());
                if (!running.load()) break;
                core::detail::drain_edits(*runtime);
                if (!running.load()) break;
                before_frame();
                core::detail::drain_posts(*runtime);
                std::size_t steps = 0;
                while (running.load() && accumulated >= settings.fixed_delta && steps < settings.max_fixed_steps) {
                    update(root().state.get(), settings.fixed_delta, true, pool);
                    {
                        std::lock_guard lock(frame_mutex);
                        ++frame_context.physics_tick;
                    }
                    ++steps;
                    accumulated -= settings.fixed_delta;
                }
                if (accumulated >= settings.fixed_delta) {
                    dropped_steps.fetch_add(static_cast<std::uint64_t>(accumulated / settings.fixed_delta));
                    accumulated = std::fmod(accumulated, settings.fixed_delta);
                }
                if (running.load()) update(root().state.get(), delta, false, pool);
                after_frame();
                if (running.load()) finish_frame();
                ++frame;
                if (settings.target_frame_rate > 0) {
                    auto period = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                        std::chrono::duration<double>(1.0 / settings.target_frame_rate));
                    wait_until(start + period);
                }
            }
        } catch (...) {
            failure = std::current_exception();
        }
        stop();
        {
            std::lock_guard lock(runtime->queue_mutex);
            runtime->accepting = false;
        }
        if (started) {
            core::detail::exit_nodes(root().state.get(), failure);
            try {
                on_stop();
            } catch (...) {
                if (!failure) failure = std::current_exception();
            }
        }
        try {
            core::detail::drain_edits(*runtime, true);
        } catch (...) {
            if (!failure) failure = std::current_exception();
        }
        try {
            core::detail::drain_posts(*runtime, true);
        } catch (...) {
            if (!failure) failure = std::current_exception();
        }
        try {
            if (session && session->has_events()) Input::instance().initialize();
        } catch (...) {
            if (!failure) failure = std::current_exception();
        }
        stop_run_services();
        main_window.reset();
        session.reset();
        {
            std::lock_guard lock(core::detail::tree_mutex);
            runtime->active = false;
        }
        {
            std::lock_guard lock(settings_mutex);
            in_run = false;
        }
        if (failure) std::rethrow_exception(failure);
    }
} // namespace gkit
