/** @file application.hpp
 * @brief Windowed application with coordinator-driven rendering.
 */
#pragma once

#include "gkit/core/processer.hpp"
#include "gkit/graphic/render/renderer.hpp"

namespace gkit::graphic {
    /**
     * @brief Run a scene tree with one OpenGL window and an application-owned renderer.
     * @note Create graphics resources after startup and submit drawing from render(). All GPU operations use
     * the coordinator thread. The renderer remains available through node exit and on_stop().
     * @note Causing application dependented on gkit::graphic, so application is under module gkit::graphic. It
     * will be moved to better place later.
     */
    class Application : public Processer {
    public:
        /**
         * @brief Configure an OpenGL window and platform video for subsequent calls to run().
         * @param options Initial window settings.
         * @throws std::invalid_argument If the requested surface is incompatible with OpenGL rendering.
         */
        explicit Application(platform::WindowOptions options = {});
        ~Application() override;

    protected:
        /**
         * @brief Access the renderer for the active run, from the coordinator thread.
         * @throws std::logic_error If the renderer is unavailable or accessed outside the coordinator thread.
         */
        [[nodiscard]] auto renderer() -> Renderer&;
        /**
         * @brief Submit drawing after after_frame() while the window is drawable.
         * @note The queue executes and the window is presented after this hook returns normally.
         * Calling stop() inside the hook completes presentation of the current frame.
         */
        virtual auto render() -> void {}

    private:
        auto start_run_services() -> void override final;
        auto finish_frame() -> void override final;
        auto stop_run_services() noexcept -> void override final;

        struct Session;
        std::unique_ptr<Session> session;
    };
} // namespace gkit::graphic
