/**
 * @file processer.hpp
 * @brief Generic Node-tree lifetime, event dispatch, frame timing, and parallel update scheduling.
 */
#pragma once

#include "gkit/core/node.hpp"
#include "gkit/platform/runtime.hpp"

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>

namespace gkit::core::detail {
    struct TreeRuntime;
}

namespace gkit::core {
    /** @brief Frame timing for the Processer coordinator hooks. */
    struct FrameContext {
        /** @brief Elapsed time in seconds, capped by the Processer configuration. */
        double delta = 0;
        /** @brief Zero-based logic-frame number, reset on each run. */
        std::uint64_t frame = 0;
        /** @brief Number of completed physics steps in this run. */
        std::uint64_t physics_tick = 0;
    };
} // namespace gkit::core

namespace gkit {
    /**
     * @brief Owns a non-null generic Node root and manages its complete running-tree lifetime.
     * @note Construct and run on the same thread, which must be SDL's main thread when SDL is enabled.
     * Derive to customize coordinator hooks; run() itself owns ordering, barriers, cleanup and exception propagation.
     * A Processer must outlive its running loop. Call stop() from other threads, then wait for run() to return before
     * destroying it. Lifecycle and coordinator hooks may access this tree while all workers are idle.
     * @note Parallel marking is opt-in per subtree. Library node properties can be accessed concurrently.
     * Synchronize custom node fields or use core::Node::post() for coordinator access.
     */
    class Processer {
    public:
        /** @brief Create the root and stopped runtime; SDL and worker initialization are deferred to run(). */
        Processer();
        /**
         * @brief Cancel queued requests and release the owned tree after run() has returned.
         * @note Destruction does not join a run() executing on another thread; callers must synchronize its lifetime.
         * Exceptions from cancellation callbacks are suppressed during destruction.
         */
        virtual ~Processer();
        Processer(const Processer&)                    = delete;
        auto operator=(const Processer&) -> Processer& = delete;
        /** @brief Return the root owned by this Processer; attach application nodes beneath this root. */
        auto root() noexcept -> core::Node&;
        auto root() const noexcept -> const core::Node&;
        /** @brief Enable worker updates for explicitly marked independent subtrees on the next run. */
        auto set_parallel_update_enabled(bool value) noexcept -> Processer&;
        /** @brief Set the number of workers, excluding the coordinator. Parallel updates require at least one. */
        auto set_worker_count(std::size_t value) noexcept -> Processer&;
        /** @brief Set the physics step in seconds to a positive finite value. */
        auto set_fixed_delta(double value) noexcept -> Processer&;
        /** @brief Set the maximum number of physics ticks per frame to a positive value. */
        auto set_max_fixed_steps(std::size_t value) noexcept -> Processer&;
        /** @brief Set a positive finite upper bound on measured frame time in seconds. */
        auto set_max_frame_delta(double value) noexcept -> Processer&;
        /** @brief Set the target logic-frame rate. Zero disables pacing. */
        auto set_target_frame_rate(double value) noexcept -> Processer&;
        /** @brief Set typed options for subsequent runs. These take precedence over legacy SDL setters. */
        auto set_platform_options(platform::RuntimeOptions value) -> Processer&;
        /** @brief Enable SDL initialization and event polling for the configured subsystems. */
        auto set_initialize_sdl(bool value) noexcept -> Processer&;
        /** @brief Select SDL subsystems. std::nullopt enables audio, events, gamepad, joystick and video. */
        auto set_sdl_flags(std::optional<std::uint32_t> value) noexcept -> Processer&;
        /**
         * @brief Enter the tree, process frames until stopped, and leave the tree before returning.
         * @note Each frame polls events and updates Input when SDL events are enabled, then enters the tree on the
         * first frame, commits queued edits, invokes before_frame(), drains posted callbacks, runs bounded fixed
         * physics steps and one variable logic step, then invokes after_frame() and paces.
         * All update tasks finish before each phase boundary. Excess whole physics steps are discarded; fractional
         * remainder is retained. Settings remain fixed throughout this call.
         * @throws std::logic_error For re-entry or execution on the wrong thread.
         * @throws std::invalid_argument If the configured timing or worker count is invalid.
         * @throws std::runtime_error If SDL initialization fails.
         * @note Allocation, worker creation and callback exceptions propagate after workers finish, entered nodes exit,
         * pending requests are cancelled and acquired SDL subsystems are released. The first failure is preserved.
         * May be called again after returning; frame counters reset, enter_tree() repeats and ready() does not.
         */
        auto run() -> void;
        /**
         * @brief Request termination and wake any frame-pacing wait, from any thread.
         * @note Idempotent. Current callbacks finish; queued operations not committed before termination are cancelled.
         * This does not destroy the tree or join the calling thread. A later run() starts a new running lifetime.
         */
        auto stop() noexcept -> void;
        /** @brief Read the number of whole accumulated ticks discarded by the catch-up cap in the latest run. */
        auto dropped_physics_steps() const noexcept -> std::uint64_t { return dropped_steps.load(); }

    protected:
        /** @brief Coordinator hook after startup succeeds and before parent-first tree entry. */
        virtual auto on_start() -> void {}
        /**
         * @brief Coordinator hook after input and edits, before posted callbacks and updates.
         * @param context This frame's measured delta and counters.
         * @note All workers are idle; direct access to this tree is permitted.
         */
        virtual auto before_frame(const core::FrameContext& context) -> void {}
        /**
         * @brief Coordinator hook after all scheduled tasks finish, before pacing.
         * @param context This frame's timing, including the completed physics-tick count.
         * @note May also run when stop() skipped part of the current frame; not called after an update exception.
         */
        virtual auto after_frame(const core::FrameContext& context) -> void {}
        /** @brief Coordinator cleanup hook after tree exit, including a failed startup hook or update. */
        virtual auto on_stop() -> void {}
        /**
         * @brief Read a monotonic time point; defaults to std::chrono::steady_clock::now().
         * @return Current time in the scheduler clock's epoch.
         * @note Override together with wait_until() for deterministic timing tests.
         */
        virtual auto now() const -> std::chrono::steady_clock::time_point;
        /**
         * @brief Wait for a frame deadline or stop(), whichever happens first.
         * @param deadline Absolute time in the same epoch as now(). Past deadlines return immediately.
         * @note Test overrides must preserve stop responsiveness; production uses a condition variable.
         */
        virtual auto wait_until(std::chrono::steady_clock::time_point deadline) -> void;

    private:
        struct Config {
            bool parallel_update_enabled = false;
            std::size_t worker_count     = 1;
            double fixed_delta           = 1.0 / 60.0;
            std::size_t max_fixed_steps  = 4;
            double max_frame_delta       = 0.25;
            double target_frame_rate     = 60;
            std::optional<platform::RuntimeOptions> platform_options;
            bool initialize_sdl = true;
            std::optional<std::uint32_t> sdl_flags;
        };

        core::UniqueObject root_owner;
        std::unique_ptr<core::detail::TreeRuntime> runtime;
        Config config;
        mutable std::mutex settings_mutex;
        std::mutex wait_mutex;
        std::condition_variable wake;
        std::atomic<bool> running                = false;
        bool in_run                              = false;
        std::atomic<std::uint64_t> dropped_steps = 0;
    };
} // namespace gkit
