/** @file application.hpp
 * @brief Windowed application with coordinator-driven rendering.
 */
#pragma once

#include "gkit/core/processer.hpp"
#include "gkit/graphic/render/renderer.hpp"
#include "gkit/graphic/window.hpp"

namespace gkit::graphic {
    /**
     * @brief Automatically draw the main window's subtree.
     * The main window starts as a child named "window". Detaching it suspends rendering while simulation continues.
     * Reattaching that node opens a fresh graphics session. All GPU work runs on the coordinator.
     */
    class Application : public Processer {
    public:
        /** @brief Attach the default window node. Native window creation is deferred until run(). */
        Application();
        /** @brief Configure the attached window's title for its next opening. Throws if no window is attached. */
        auto set_window_title(std::string title) -> Application&;
        /** @brief Configure the attached window's logical size for its next opening. */
        auto set_window_size(int width, int height) -> Application&;
        /** @brief Transfer a Scene subtree to the main window. Switches commit at the next edit boundary while running.
         * A replacement keeps the old scene's sibling position and name. Other window children are preserved.
         * With a completion, rejected and cancelled requests return the submitted owner. Without one,
         * invalid edits and a missing main window throw when the edit is processed. */
        auto set_main_scene(core::UniqueObject scene, core::Node::Completion complete = {}) -> void;
        /** @brief Request a graphics session for the attached main window at the next coordinator boundary.
         * Closing the native window preserves the scene tree. Repeated opens of an active window are harmless. */
        auto open_main_window() -> void;
        ~Application() override;

    protected:
        /** @brief Borrow the active native window on the coordinator for this session. */
        auto window() -> platform::Window&;
        /**
         * @brief Access the renderer for the active run, from the coordinator thread.
         * @throws std::logic_error If the renderer is unavailable or accessed outside the coordinator thread.
         */
        [[nodiscard]] auto renderer() -> Renderer&;
        /**
         * @brief Optional custom drawing after after_frame(). Sprites are submitted automatically.
         * @note The queue executes and the window is presented after this hook returns normally.
         * Calling stop() inside the hook completes presentation of the current frame.
         */
        virtual auto render() -> void {}

    private:
        /** @brief Check whether the designated window belongs to this tree. Caller holds the tree mutex. */
        auto main_window_attached() const -> bool;
        auto update_window_attachment(Window& node, bool entering) -> void;
        auto open_window_if_attached() -> void;
        auto open_window(Window& node) -> void;
        auto handle_events(const std::vector<platform::Event>& events) -> void override final;
        auto start_run_services(const platform::Runtime* runtime) -> void override final;
        auto finish_frame() -> void override final;
        auto stop_run_services() noexcept -> void override final;

        core::ObjectId main_window_id;
        bool video_enabled  = false;
        bool open_on_attach = false;
        struct Session;
        std::unique_ptr<Session> session;
    };
} // namespace gkit::graphic
