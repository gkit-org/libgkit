/** @file processer.cpp
 * @brief Persistent worker scheduling, frame barriers and exception-safe run cleanup.
 */
#include "gkit/core/processer.hpp"

#include "core/misc/sdl_event_dispatcher.hpp"
#include "core/node_internal.hpp"

#include <SDL3/SDL.h>

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
         * @brief Partition a stable tree into non-overlapping update domains before starting a phase.
         * @note The root domain belongs to the coordinator. Nested parallel marks inherit the outermost domain;
         * disabling Processer parallelism places every node in the coordinator domain.
         */
        auto assign_domains(const std::shared_ptr<State>& root, bool parallel) -> void {
            std::lock_guard lock(core::detail::tree_mutex);
            std::vector<std::pair<std::shared_ptr<State>, State*>> pending{{root, root.get()}};
            while (!pending.empty()) {
                auto [current, domain] = std::move(pending.back());
                pending.pop_back();
                if (parallel && current != root && domain == root.get() && current->node().is_parallel())
                    domain = current.get();
                current->domain = domain;
                for (const auto& child : current->children) pending.push_back({child, domain});
            }
        }
        /**
         * @brief Walk one worker-owned subtree parent-first, applying inherited update gates.
         * @note User code may post work or stage edits, but cannot mutate topology or directly enter other domains.
         */
        auto serial_update(const std::shared_ptr<State>& root, const core::FrameContext& context, bool physics)
            -> void {
            core::detail::AccessScope access(root->domain);
            std::vector<std::shared_ptr<State>> pending{root};
            while (!pending.empty()) {
                auto current = std::move(pending.back());
                pending.pop_back();
                auto& node = current->node();
                if (!node.updates_enabled()) continue;
                if (physics)
                    node.physics_process(context);
                else
                    node.process(context);
                if (!node.updates_enabled()) continue;
                pending.insert(pending.end(), current->children.rbegin(), current->children.rend());
            }
        }
        /**
         * @brief Update coordinator nodes and dispatch each independent subtree after its ancestors have completed.
         * @note Always join the phase before returning or throwing. No ordering is promised between independent
         * domains; local traversal remains insertion-ordered. This barrier separates every physics and logic phase.
         */
        auto update(const std::shared_ptr<State>& root,
                    const core::FrameContext& context,
                    bool physics,
                    WorkerPool& pool) -> void {
            core::detail::AccessScope access(root.get());
            std::exception_ptr failure;
            try {
                std::vector<std::shared_ptr<State>> pending{root};
                while (!pending.empty()) {
                    pool.check_failure();
                    auto current = std::move(pending.back());
                    pending.pop_back();
                    if (current->domain != root.get()) {
                        pool.submit([current, context, physics] {
                            serial_update(current, context, physics);
                        });
                        continue;
                    }
                    auto& node = current->node();
                    if (!node.updates_enabled()) continue;
                    if (physics)
                        node.physics_process(context);
                    else
                        node.process(context);
                    if (!node.updates_enabled()) continue;
                    pending.insert(pending.end(), current->children.rbegin(), current->children.rend());
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
        root_owner(core::NodeOwner<>::create()), runtime(std::make_shared<core::detail::TreeRuntime>()) {
        std::lock_guard lock(core::detail::tree_mutex);
        core::detail::assign_runtime(root_owner.state, runtime);
    }
    Processer::~Processer() {
        // Destruction must follow run() returning; stop() is the cross-thread operation.
        {
            std::lock_guard lock(runtime->queue_mutex);
            runtime->accepting = false;
        }
        core::detail::AccessScope access(nullptr, runtime.get());
        try {
            core::detail::drain_edits(runtime, true);
        } catch (...) {
        }
        try {
            core::detail::drain_posts(runtime, true);
        } catch (...) {
        }
    }
    auto Processer::root() const -> core::NodeHandle<> {
        return root_owner.handle();
    }
    auto Processer::configure(const ProcesserSettings& value) -> void {
        std::lock_guard lock(settings_mutex);
        if (in_run) throw std::logic_error("configure before run");
        if (!std::isfinite(value.fixed_delta) || value.fixed_delta <= 0 || value.max_fixed_steps == 0 ||
            !std::isfinite(value.max_frame_delta) || value.max_frame_delta <= 0 ||
            !std::isfinite(value.target_frame_rate) || value.target_frame_rate < 0 ||
            (value.parallel_update_enabled && value.worker_count == 0) ||
            value.max_frame_delta / value.fixed_delta >=
                static_cast<double>(std::numeric_limits<std::uint64_t>::max()) ||
            (value.target_frame_rate > 0 &&
             1.0 / value.target_frame_rate >
                 std::chrono::duration<double>(std::chrono::steady_clock::duration::max()).count() / 2))
            throw std::invalid_argument("invalid frame settings");
        config = value;
    }
    auto Processer::settings() const -> ProcesserSettings {
        std::lock_guard lock(settings_mutex);
        return config;
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
    /**
     * @brief Execute the scheduler's startup, frame loop and unconditional cleanup boundary.
     * @note Workers are scoped inside the guarded run body, so they finish before tree exit callbacks run.
     * Shutdown closes queues before callbacks, preserves the first exception, releases only acquired SDL subsystems,
     * and clears the re-entry guard only after cleanup. Remaining whole physics backlog is counted and discarded.
     */
    auto Processer::run() -> void {
        ProcesserSettings settings;
        {
            std::lock_guard lock(settings_mutex);
            if (in_run) throw std::logic_error("run is not reentrant");
            if (runtime->thread != std::this_thread::get_id()) throw std::logic_error("run on the creating thread");
            in_run   = true;
            settings = config;
            running.store(true);
        }
        dropped_steps.store(0);
        core::detail::AccessScope access(nullptr, runtime.get());
        std::exception_ptr failure;
        SDL_InitFlags flags                        = 0;
        core::SDLEventDispatcher::Token quit_token = 0;
        bool started                               = false;
        try {
            WorkerPool pool(settings.parallel_update_enabled ? settings.worker_count : 0);
            if (settings.initialize_sdl) {
                auto requested = settings.sdl_flags.value_or(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD |
                                                             SDL_INIT_JOYSTICK | SDL_INIT_VIDEO);
                if (!SDL_IsMainThread()) throw std::logic_error("SDL requires the main thread");
                if (!SDL_InitSubSystem(requested)) throw std::runtime_error(SDL_GetError());
                flags = requested;
                if (flags & SDL_INIT_EVENTS) {
                    Input::instance().initialize();
                    quit_token = core::SDLEventDispatcher::instance().register_event_handler(SDL_EVENT_QUIT,
                                                                                             [this](const auto&) {
                                                                                                 stop();
                                                                                             });
                }
            }
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
            core::detail::enter_nodes(root_owner.state);
            auto previous      = now();
            double accumulated = 0;
            core::FrameContext context;
            auto empty_input = std::make_shared<const input::Snapshot>();
            while (running.load()) {
                auto start = now();
                context.delta =
                    std::clamp(std::chrono::duration<double>(start - previous).count(), 0.0, settings.max_frame_delta);
                previous = start;
                accumulated += context.delta;
                core::detail::drain_edits(runtime);
                if (flags & SDL_INIT_EVENTS) core::SDLEventDispatcher::instance().dispatch_events();
                context.input = (flags & SDL_INIT_EVENTS) ? Input::instance().snapshot() : empty_input;
                if (!running.load()) break;
                before_frame(context);
                core::detail::drain_posts(runtime);
                assign_domains(root_owner.state, settings.parallel_update_enabled);
                std::size_t steps = 0;
                while (running.load() && accumulated >= settings.fixed_delta && steps < settings.max_fixed_steps) {
                    auto fixed  = context;
                    fixed.delta = settings.fixed_delta;
                    update(root_owner.state, fixed, true, pool);
                    ++context.physics_tick;
                    ++steps;
                    accumulated -= settings.fixed_delta;
                }
                if (accumulated >= settings.fixed_delta) {
                    dropped_steps.fetch_add(static_cast<std::uint64_t>(accumulated / settings.fixed_delta));
                    accumulated = std::fmod(accumulated, settings.fixed_delta);
                }
                if (running.load()) update(root_owner.state, context, false, pool);
                after_frame(context);
                ++context.frame;
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
            core::detail::exit_nodes(root_owner.state, failure);
            try {
                on_stop();
            } catch (...) {
                if (!failure) failure = std::current_exception();
            }
        }
        try {
            core::detail::drain_edits(runtime, true);
        } catch (...) {
            if (!failure) failure = std::current_exception();
        }
        try {
            core::detail::drain_posts(runtime, true);
        } catch (...) {
            if (!failure) failure = std::current_exception();
        }
        if (quit_token) core::SDLEventDispatcher::instance().disconnect(quit_token);
        if (flags) SDL_QuitSubSystem(flags);
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
