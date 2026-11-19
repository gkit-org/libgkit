#include "gkit/graphic/application.hpp"

#include "core/node_internal.hpp"
#include "gkit/scene/scene.hpp"
#include "graphic/render_device.hpp"
#include "platform/window_close_policy.hpp"

#include <utility>

namespace gkit::graphic {
    struct Application::Session {
        platform::detail::KeepRunningOnWindowClose close_policy;
        platform::Window window;
        Renderer renderer;

        explicit Session(const platform::WindowOptions& options) :
            window(options), renderer(RenderDevice::create_for_window(window)) {}
    };

    Application::Application() {
        set_platform_options({.video = true});
        auto main_window      = core::UniqueObject::create<Window>();
        main_window_id        = main_window.get_id();
        auto& node            = *static_cast<Window*>(main_window.get());
        node.application_root = root().id();
        node.tree_changed     = [this](Window& window, bool entering) -> void {
            update_window_attachment(window, entering);
        };
        root().add_child("window", std::move(main_window));
    }


    auto Application::main_window_attached() const -> bool {
        const auto root_id = root().id();
        for (auto* node = core::detail::NodeState::resolve(main_window_id); node;
             node       = core::detail::NodeState::resolve(node->parent))
            if (node->id == root_id) return true;
        return false;
    }


    auto Application::update_window_attachment(Window& node, bool entering) -> void {
        if (entering) {
            if (open_on_attach) open_window(node);
            return;
        }
        {
            std::lock_guard lock(core::detail::tree_mutex);
            // During run shutdown the window stays in the tree and on_stop() can still use it.
            if (main_window_attached()) return;
        }
        stop_run_services();
        open_on_attach = true;
    }


    auto Application::set_window_title(std::string title) -> Application& {
        std::lock_guard lock(core::detail::tree_mutex);
        if (!main_window_attached()) throw std::logic_error("no attached main window");
        main_window_id.with<Window>([&](auto& node) -> void {
            node.set_title(std::move(title));
        });
        return *this;
    }


    auto Application::set_window_size(int width, int height) -> Application& {
        std::lock_guard lock(core::detail::tree_mutex);
        if (!main_window_attached()) throw std::logic_error("no attached main window");
        main_window_id.with<Window>([&](auto& node) -> void {
            node.set_size(width, height);
        });
        return *this;
    }


    auto Application::set_main_scene(core::UniqueObject scene, core::Node::Completion complete) -> void {
        if (!dynamic_cast<scene::Scene*>(scene.get())) {
            if (!complete) throw std::invalid_argument("main scene requires a Scene");
            complete({core::NodeError::InvalidEdit, std::move(scene)});
            return;
        }
        if (!complete) {
            complete = [](core::EditResult result) -> void {
                if (result.error == core::NodeError::Expired) throw std::logic_error("no attached main window");
                if (result.error == core::NodeError::InvalidEdit)
                    throw std::invalid_argument("invalid main scene edit");
            };
        }
        core::detail::Edit edit{
            core::detail::EditKind::SetChild, {}, scene.get_id(), "main_scene", std::move(complete), std::move(scene)};
        std::unique_lock lock(core::detail::tree_mutex);
        if (main_window_attached()) edit.anchor = main_window_id;
        edit.submit(lock);
    }


    auto Application::open_main_window() -> void {
        bool already_open = false;
        {
            std::lock_guard lock(core::detail::tree_mutex);
            if (!main_window_attached()) throw std::logic_error("no attached main window");
            main_window_id.with<Window>([&](Window& node) -> void {
                already_open = node.state().has_value();
            });
        }
        if (already_open) return;
        root().post([this](core::Node&) -> void {
            open_window_if_attached();
        });
    }


    auto Application::window() -> platform::Window& {
        platform::require_main_thread();
        if (!session) throw std::logic_error("no active window");
        return session->window;
    }


    Application::~Application() = default;


    auto Application::renderer() -> Renderer& {
        platform::require_main_thread();
        if (!session) throw std::logic_error("the application has no active renderer");
        session->renderer.require_context();
        return session->renderer;
    }


    auto Application::start_run_services(const platform::Runtime* runtime) -> void {
        video_enabled  = runtime && runtime->has_video();
        open_on_attach = true;
        open_window_if_attached();
    }


    auto Application::open_window_if_attached() -> void {
        if (session) return;
        {
            std::lock_guard lock(core::detail::tree_mutex);
            if (!main_window_attached()) return;
        }
        main_window_id.with<Window>([this](Window& node) -> void {
            open_window(node);
        });
    }


    auto Application::open_window(Window& node) -> void {
        if (!video_enabled) throw std::invalid_argument("window nodes require platform video");
        platform::WindowOptions options;
        {
            std::lock_guard lock(node.mutex);
            options = node.options;
        }
        session        = std::make_unique<Session>(options);
        open_on_attach = false;
        std::lock_guard lock(node.mutex);
        node.current = window().state();
    }


    auto Application::handle_events(const std::vector<platform::Event>& events) -> void {
        if (!session) return;
        for (const auto& event : events) {
            const auto* change = std::get_if<platform::WindowEvent>(&event);
            if (!change || change->window != window().state().id) continue;
            if (change->change == platform::WindowChange::Close) {
                stop_run_services();
                return;
            }
            window().refresh();
            main_window_id.with<Window>([&](Window& node) -> void {
                std::lock_guard lock(node.mutex);
                node.current = window().state();
            });
        }
    }


    auto Application::finish_frame() -> void {
        if (!session) return;
        const auto state = window().state();
        if (!state.drawable()) return;
        auto& active_renderer = renderer();
        active_renderer.begin_frame();
        try {
            active_renderer.clear();
            render();
            main_window_id.with<Window>([&](Window& main) -> void {
                active_renderer.draw_sprites(main, state.width, state.height, state.pixel_width, state.pixel_height);
            });
            active_renderer.finish_frame({0, 0, state.pixel_width, state.pixel_height});
        } catch (...) {
            active_renderer.discard_frame();
            throw;
        }
    }


    auto Application::stop_run_services() noexcept -> void {
        if (session)
            main_window_id.with<Window>([](auto& node) -> void {
                std::lock_guard lock(node.mutex);
                node.current.reset();
            });
        session.reset();
    }
} // namespace gkit::graphic
