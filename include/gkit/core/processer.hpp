/**
 * @file processer.hpp
 * @brief Generic Node-tree lifetime, event dispatch, frame timing, and parallel update scheduling.
 */
#pragma once

#include "gkit/core/node.hpp"

#include <chrono>
#include <condition_variable>
#include <mutex>

namespace gkit {
    /**
     * @brief Scheduler configuration copied by Processer::configure() before run().
     * @note Invalid timing values are rejected before starting workers or initializing SDL.
     */
    struct ProcesserSettings {
        bool parallel_update_enabled = false; ///< Run explicitly marked independent subtrees on workers.
        std::size_t worker_count =
            1; ///< Worker count, excluding the coordinator; positive when parallelism is enabled.
        double fixed_delta          = 1.0 / 60.0; ///< Positive finite physics step in seconds.
        std::size_t max_fixed_steps = 4; ///< Maximum physics ticks per frame; must be positive.
        double max_frame_delta      = 0.25; ///< Positive finite clamp for measured elapsed time, in seconds.
        double target_frame_rate    = 60; ///< Finite non-negative logic-frame rate; zero disables pacing.
        bool initialize_sdl         = true; ///< Disable for a headless scheduler with an empty input snapshot.
        /// SDL_InitFlags override. nullopt initializes audio, events, gamepad, joystick and video as before.
        std::optional<std::uint32_t> sdl_flags;
    };

    /**
     * @brief Owns a non-null generic Node root and manages its complete running-tree lifetime.
     * @note Construct and run on the same thread, which must be SDL's main thread when SDL is enabled.
     * Derive to customize coordinator hooks; run() itself owns ordering, barriers, cleanup and exception propagation.
     * A Processer must outlive its running loop. Call stop() from other threads, then wait for run() to return before
     * destroying it. Lifecycle and coordinator hooks may access this tree while all workers are idle.
     * @note Parallel marking is opt-in per subtree. Mutable node state is confined to its execution domain;
     * use core::NodeHandle::post() or core::PublishedState across domains. The core module does not depend on scene.
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
        /** @brief Return the always-present root identity; attach application nodes beneath this root. */
        auto root() const -> core::NodeHandle<>;
        /**
         * @brief Validate and copy all scheduler settings atomically.
         * @param value Configuration used by subsequent run() calls.
         * @throws std::logic_error If a run is active, including its cleanup phase.
         * @throws std::invalid_argument For invalid steps/rates, an unrepresentable duration, or zero enabled workers.
         * @note Thread-safe. Failed configuration leaves the previous settings unchanged.
         */
        auto configure(const ProcesserSettings& value) -> void;
        /** @brief Return a thread-safe copy of the configured settings. */
        auto settings() const -> ProcesserSettings;
        /**
         * @brief Enter the tree, process frames until stopped, and leave the tree before returning.
         * @note Each frame commits queued edits, dispatches SDL events, invokes before_frame(), drains posted callbacks,
         * runs bounded fixed physics steps, runs one variable logic step, then invokes after_frame() and paces.
         * All update tasks finish before each phase boundary. Excess whole physics steps are discarded; fractional
         * remainder is retained. Settings remain fixed throughout this call.
         * @throws std::logic_error For re-entry or execution on the wrong thread.
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
         * @brief Coordinator hook after edits and event dispatch, before posted callbacks and updates.
         * @param context This frame's measured delta, counters and immutable input.
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
        core::NodeOwner<> root_owner;
        std::shared_ptr<core::detail::TreeRuntime> runtime;
        ProcesserSettings config;
        mutable std::mutex settings_mutex;
        std::mutex wait_mutex;
        std::condition_variable wake;
        std::atomic<bool> running                = false;
        bool in_run                              = false;
        std::atomic<std::uint64_t> dropped_steps = 0;
    };
} // namespace gkit
