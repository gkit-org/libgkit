#include "gkit/core/input/input.hpp"
#include "gkit/graphic/application.hpp"
#include "gkit/graphic/config.hpp"
#include "gkit/graphic/render/renderer.hpp"
#include "gkit/graphic/vertex_buffer_layout.hpp"
#include "gkit/scene/2d/sprite2d.hpp"
#include "gkit/scene/scene.hpp"
#include "graphic/sprite_pass.hpp"
#include "test_utils.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>

#include <array>
#include <filesystem>
#include <iostream>

#include <glad/gl.h>

namespace fs = std::filesystem;
namespace {
    struct DemoResources {
        std::shared_ptr<gkit::graphic::FrameBuffer> fbo;
        std::shared_ptr<gkit::graphic::RenderObject> triangle_obj, quad_obj, alpha_triangle_obj, stencil_triangle_obj,
            masked_triangle_obj;
        DemoResources(gkit::graphic::Renderer& renderer, int screen_width, int screen_height) {
            const fs::path resource_base = fs::path(__FILE__).parent_path();
            // Colored triangle vertex data (position + color)
            std::vector<float> tri_vertices = {// positions            // colors
                                               0.0f,
                                               0.5f,
                                               0.0f,
                                               1.0f,
                                               0.0f,
                                               0.0f, // top: red
                                               -0.4f,
                                               -0.25f,
                                               0.0f,
                                               0.0f,
                                               1.0f,
                                               0.0f, // bottom-left: green
                                               0.4f,
                                               -0.25f,
                                               0.0f,
                                               0.0f,
                                               0.0f,
                                               1.0f}; // bottom-right: blue

            std::vector<uint32_t> tri_indices = {0, 1, 2};

            gkit::graphic::VertexBufferLayout tri_layout;
            tri_layout.push<float>(3); // position
            tri_layout.push<float>(3); // color

            // load shader source
            auto tri_shader = renderer.create_shader((resource_base / "color_triangle.shader").string());

            // Full-screen quad vertex data (post-processing).
            // z = 0.2 (farthest): it depth-tests first and writes the deepest value,
            // so the later triangles (z = 0.1 and z = 0) depth-test against it.
            std::vector<float> quad_vertices = {// positions                    // tex coords
                                                -1.0f, -1.0f, 0.2f, 0.0f, 0.0f, 1.0f,  -1.0f, 0.2f, 1.0f, 0.0f,
                                                1.0f,  1.0f,  0.2f, 1.0f, 1.0f, -1.0f, 1.0f,  0.2f, 0.0f, 1.0f};

            std::vector<uint32_t> quad_indices = {0, 1, 2, 2, 3, 0};

            gkit::graphic::VertexBufferLayout quad_layout;
            quad_layout.push<float>(3);
            quad_layout.push<float>(2);

            // load post-processing shader
            auto post_shader = renderer.create_shader((resource_base / "post_process.shader").string());

            // load alpha-blended triangle shader (u_alpha uniform controls opacity)
            auto alpha_shader = renderer.create_shader((resource_base / "alpha_triangle.shader").string());

            fbo              = renderer.create_frame_buffer(screen_width, screen_height);
            auto fbo_texture = renderer.create_color_attachment(screen_width, screen_height);
            fbo->attach_color_texture(fbo_texture, 0);
            fbo->attach_depth_stencil(renderer.create_render_buffer(screen_width, screen_height));
            fbo->check();

            // Reusable draw units: user provides data arrays + material; VAO/VBO/IBO hidden.

            // Triangle material
            gkit::graphic::Material tri_material;
            tri_material.set_shader(tri_shader);

            // Post-processing quad material (samples the FBO texture).
            // set_texture assigns the next free slot automatically.
            gkit::graphic::Material post_material;
            post_material.set_shader(post_shader);
            post_material.set_texture(fbo_texture);
            post_material.set_uniform("screenTexture", 0);

            // Opaque objects enable depth testing so they write their depth and the
            // translucent triangle is really depth-tested against them (a disabled
            // depth test does not write the depth buffer, which would leave it empty).
            triangle_obj =
                std::make_shared<gkit::graphic::RenderObject>(tri_vertices, tri_indices, tri_layout, tri_material);
            triangle_obj->state.depth.enabled = true;

            quad_obj =
                std::make_shared<gkit::graphic::RenderObject>(quad_vertices, quad_indices, quad_layout, post_material);
            quad_obj->state.depth.enabled = true;

            // Transparent triangle: reuses the triangle's shape (positions + colors),
            // offset 50 px toward the bottom-left (NDC: 500 px window → 1 px = 0.004,
            // 50 px = 0.2). z = 0.1 sits between the quad (z = 0.2, farthest) and the
            // comparison triangle (z = 0, nearest), so depth testing decides the layering
            // among the three. It blends with what is already on screen
            // (SrcAlpha / OneMinusSrcAlpha, u_alpha = 0.8 → 0.8·src + 0.2·dst).
            const float px_to_ndc = 2.0f / static_cast<float>(screen_width); // 1 px in NDC
            const float offset_x  = -50.0f * px_to_ndc;
            const float offset_y  = -50.0f * px_to_ndc;
            std::vector<float> alpha_vertices;
            for (std::size_t v = 0; v < tri_vertices.size(); v += 6) {
                alpha_vertices.push_back(tri_vertices[v] + offset_x); // position x
                alpha_vertices.push_back(tri_vertices[v + 1] + offset_y); // position y
                alpha_vertices.push_back(0.1f); // position z: between quad (0.2) and comparison triangle (0)
                // reuse the original colors (indices 3..5)
                alpha_vertices.insert(alpha_vertices.end(), tri_vertices.begin() + v + 3, tri_vertices.begin() + v + 6);
            }

            gkit::graphic::Material alpha_material;
            alpha_material.set_shader(alpha_shader);
            alpha_material.set_uniform("u_alpha", 0.8f);

            alpha_triangle_obj =
                std::make_shared<gkit::graphic::RenderObject>(alpha_vertices, tri_indices, tri_layout, alpha_material);
            alpha_triangle_obj->state.depth.enabled   = true; // depth-tests against quad + comparison triangle
            alpha_triangle_obj->state.blend.enabled   = true;
            alpha_triangle_obj->state.blend.src_rgb   = gkit::graphic::BlendFunc::SrcAlpha;
            alpha_triangle_obj->state.blend.dst_rgb   = gkit::graphic::BlendFunc::OneMinusSrcAlpha;
            alpha_triangle_obj->state.blend.src_alpha = gkit::graphic::BlendFunc::SrcAlpha;
            alpha_triangle_obj->state.blend.dst_alpha = gkit::graphic::BlendFunc::OneMinusSrcAlpha;
            alpha_triangle_obj->transparent           = true; // sorted after opaque, back-to-front
            alpha_triangle_obj->depth_key             = 0.0f; // nearest transparent → drawn last

            // Stencil-mask triangle: offset 50 px right and 50 px up, written into the
            // FBO as a stencil=1 region (color is irrelevant — the FBO color is cleared
            // right after). Depth test stays disabled so it writes no depth and can't
            // reject the later masked triangle.
            const float right_offset = 50.0f * px_to_ndc;
            const float up_offset    = 35.0f * px_to_ndc;
            std::vector<float> stencil_vertices;
            for (std::size_t v = 0; v < tri_vertices.size(); v += 6) {
                stencil_vertices.push_back(tri_vertices[v] + right_offset); // position x + 50 px
                stencil_vertices.push_back(tri_vertices[v + 1] + up_offset); // position y + 50 px
                stencil_vertices.push_back(tri_vertices[v + 2]); // position z unchanged
                // reuse the original colors (indices 3..5)
                stencil_vertices.insert(
                    stencil_vertices.end(), tri_vertices.begin() + v + 3, tri_vertices.begin() + v + 6);
            }

            stencil_triangle_obj =
                std::make_shared<gkit::graphic::RenderObject>(stencil_vertices, tri_indices, tri_layout, tri_material);
            stencil_triangle_obj->state.stencil.enabled = true;
            stencil_triangle_obj->state.stencil.compare_func =
                gkit::graphic::CompareFunc::Always; // always pass, just write
            stencil_triangle_obj->state.stencil.ref        = 1;
            stencil_triangle_obj->state.stencil.write_mask = 0xFF;
            stencil_triangle_obj->state.stencil.fail       = gkit::graphic::StencilOp::Keep;
            stencil_triangle_obj->state.stencil.z_fail     = gkit::graphic::StencilOp::Keep;
            stencil_triangle_obj->state.stencil.z_pass     = gkit::graphic::StencilOp::Replace; // write stencil=ref=1

            // Triangle drawn after the stencil mask: NotEqual(1) rejects fragments
            // inside the mask region, leaving a hole there and drawing elsewhere.
            masked_triangle_obj =
                std::make_shared<gkit::graphic::RenderObject>(tri_vertices, tri_indices, tri_layout, tri_material);
            masked_triangle_obj->state.depth.enabled        = true;
            masked_triangle_obj->state.stencil.enabled      = true;
            masked_triangle_obj->state.stencil.compare_func = gkit::graphic::CompareFunc::Notequal; // stencil==1 fails
            masked_triangle_obj->state.stencil.ref          = 1;
            masked_triangle_obj->state.stencil.fail         = gkit::graphic::StencilOp::Keep;
            masked_triangle_obj->state.stencil.z_fail       = gkit::graphic::StencilOp::Keep;
            masked_triangle_obj->state.stencil.z_pass       = gkit::graphic::StencilOp::Keep;
        }
        auto draw(gkit::graphic::Renderer& renderer, int screen_width, int screen_height) -> void {
            // Clear the default framebuffer (screen) every frame. Depth is cleared
            // too: the translucent triangle depth-tests, so a stale depth buffer
            // would reject it on later frames.
            renderer.clear(gkit::graphic::ClearFlags::ColorDepth);

            // Submit reusable render objects; The application executes the queue at the end of render().
            // FBO stencil-mask pass:
            // Draw 1: clear the FBO (color+depth+stencil) then write stencil=1 in the
            //         mask triangle's region (offset 50 px up; color is irrelevant).
            stencil_triangle_obj->clear = true; // clears color+depth+stencil (All)
            renderer.draw_to(fbo, stencil_triangle_obj);
            // Draw 2: clear only the FBO color (keeps the stencil marks), then draw the
            //         triangle with stencil NotEqual(1): fragments inside the stencil
            //         region are rejected, leaving a hole.
            masked_triangle_obj->clear       = true;
            masked_triangle_obj->clear_flags = gkit::graphic::ClearFlags::Color;
            renderer.draw_to(fbo, masked_triangle_obj);
            // Draw 3: post-processing quad to screen (samples fbo texture)
            renderer.draw(quad_obj);
            // Draw 4:triangle to screen (no post-processing, just for comparison)
            triangle_obj->clear = false;
            renderer.draw(
                triangle_obj,
                gkit::graphic::ViewportRect{.x = 0, .y = 0, .width = screen_width / 2, .height = screen_height / 2});
            // Draw 5: translucent triangle blended over the screen (depth-tested on
            // top of the FBO quad; transparent flag sorts it last, so it's the top layer).
            renderer.draw(alpha_triangle_obj);
        }
    };
    class Demo : public gkit::graphic::Application {
    public:
        explicit Demo(bool smoke) : smoke_test(smoke) { set_window_title("OpenGL demo").set_window_size(500, 500); }

    private:
        bool smoke_test;
        std::optional<DemoResources> resources;
        int drawn_width = 0, drawn_height = 0;

        auto check_viewport() const -> void {
            if (drawn_width == 0) return;
            GLint viewport[4]{};
            glGetIntegerv(GL_VIEWPORT, viewport);
            gkit::test::assert_if(viewport[2] == drawn_width && viewport[3] == drawn_height,
                                  "screen draws must cover the current drawable");
        }
        auto on_start() -> void override {
            const auto state = window().state();
            resources.emplace(renderer(), state.pixel_width, state.pixel_height);
        }
        auto after_frame() -> void override {
            if (gkit::Input::instance().is_key_just_pressed(gkit::input::Key::Escape)) stop();
            if (smoke_test) {
                check_viewport();
                const auto frame = get_frame_context().frame;
                if (frame == 1) window().set_size(600, 360);
                if (frame >= 120) throw std::runtime_error("window resize event timed out");
            }
        }
        auto render() -> void override {
            const auto state = window().state();
            resources->draw(renderer(), state.pixel_width, state.pixel_height);
            drawn_width  = state.pixel_width;
            drawn_height = state.pixel_height;
            if (smoke_test && state.width == 600 && state.height == 360) stop();
        }
        auto on_stop() -> void override {
            if (smoke_test) check_viewport();
            resources.reset();
        }
    };

    class TestTexture final : public gkit::graphic::Texture {
        auto bind(unsigned int) const -> void override {}
        auto unbind() const -> void override {}
        auto get_width() const -> int override { return 1; }
        auto get_height() const -> int override { return 1; }
    };

    class SubmissionProbe final : public gkit::graphic::Application {
    public:
        enum class Mode {
            Draw,
            ThrowOnStart,
            ThrowOnRender,
            StopBeforeRender
        };
        Mode mode    = Mode::Draw;
        int rendered = 0, stopped = 0;
        SubmissionProbe() {
            set_window_title("Renderer lifetime test").set_window_size(96, 96);
            auto sprite = gkit::core::UniqueObject::create<gkit::scene::Sprite2D>();
            sprite.get_id().with<gkit::scene::Sprite2D>([](auto& node) {
                node.set_scale({4, 4});
                node.set_texture(fs::path(__FILE__).parent_path() / "sprite_rows.png");
            });
            auto level = gkit::core::UniqueObject::create<gkit::scene::Scene>();
            level.get_id().with<gkit::scene::Scene>([&](auto& scene) {
                scene.add_child("sprite", std::move(sprite));
            });
            set_main_scene(std::move(level));
        }

    private:
        std::shared_ptr<gkit::graphic::Shader> shader;
        std::shared_ptr<gkit::graphic::RenderObject> reusable;
        std::shared_ptr<gkit::graphic::FrameBuffer> target;
        std::shared_ptr<gkit::graphic::Texture> color;
        std::weak_ptr<gkit::graphic::RenderObject> local_object;
        std::weak_ptr<gkit::graphic::FrameBuffer> local_target;
        bool frame_hook_called = false;

        template<class Operation>
        static auto check_rejected(Operation operation) -> void {
            try {
                operation();
            } catch (const std::logic_error&) {
                return;
            }
            throw std::runtime_error("expected graphics operation to be rejected");
        }

        auto on_start() -> void override {
            using gkit::test::assert_if;
            frame_hook_called = false;
            local_object.reset();
            local_target.reset();
            if (shader) {
                assert_if(!shader->is_valid(), "shader from the previous run has expired");
                check_rejected([&] {
                    shader->bind();
                });
                check_rejected([&] {
                    color->bind(0);
                });
                check_rejected([&] {
                    target->bind();
                });
                assert_if(!reusable->is_uploaded(), "mesh upload from the previous run has expired");
            }
            check_rejected([&] {
                renderer().clear();
            });
            const auto path = fs::path(__FILE__).parent_path() / "draw_submission.shader";
            shader          = renderer().create_shader(path.string());
            gkit::graphic::Material material;
            material.set_shader(shader);
            if (!reusable) {
                gkit::graphic::VertexBufferLayout layout;
                layout.push<float>(2);
                reusable = std::make_shared<gkit::graphic::RenderObject>(std::vector<float>{-1, -1, 1, -1, 1, 1, -1, 1},
                                                                         std::vector<std::uint32_t>{0, 1, 2, 0, 2, 3},
                                                                         layout,
                                                                         material);
            }
            reusable->material = std::move(material);
            target             = renderer().create_frame_buffer(32, 16);
            color              = renderer().create_color_attachment(32, 16);
            target->attach_color_texture(color, 0);
            target->check();
            if (mode == Mode::ThrowOnStart) throw std::runtime_error("startup failure");
        }
        auto after_frame() -> void override {
            frame_hook_called = true;
            if (mode == Mode::StopBeforeRender) stop();
        }
        auto render() -> void override {
            using gkit::test::assert_if;
            assert_if(frame_hook_called, "render follows after_frame");
            ++rendered;
            auto& output = renderer();
            reusable->material.set_uniform("box", gkit::math::Vector4(-0.5f, 0.f, 0.5f, 1.f));
            reusable->material.set_uniform("tint", gkit::math::Vector3(1.f, 0.f, 0.f));
            output.draw_to(target, reusable);
            reusable->material.set_uniform("box", gkit::math::Vector4(0.5f, 0.f, 0.5f, 1.f));
            reusable->material.set_uniform("tint", gkit::math::Vector3(0.f, 1.f, 0.f));
            output.draw_to(target, reusable);
            reusable->material.set_shader({});
            {
                auto attachment  = output.create_color_attachment(8, 8);
                auto framebuffer = output.create_frame_buffer(8, 8);
                framebuffer->attach_color_texture(attachment, 0);
                framebuffer->check();
                const auto path = fs::path(__FILE__).parent_path() / "draw_submission.shader";
                gkit::graphic::Material material;
                material.set_shader(output.create_shader(path.string()));
                gkit::graphic::VertexBufferLayout layout;
                layout.push<float>(2);
                auto object = std::make_shared<gkit::graphic::RenderObject>(
                    std::vector<float>{-1, -1, 1, -1, 0, 1}, std::vector<std::uint32_t>{0, 1, 2}, layout, material);
                object->material.set_uniform("box", gkit::math::Vector4(0.f, 0.f, 1.f, 1.f));
                object->material.set_uniform("tint", gkit::math::Vector3(0.f, 0.f, 1.f));
                local_target = framebuffer;
                local_object = object;
                output.draw_to(framebuffer, object);
            }
            assert_if(!local_target.expired() && !local_object.expired(), "queue holds local resources");
            if (mode == Mode::ThrowOnRender) throw std::runtime_error("render failure");
            stop();
        }
        auto on_stop() -> void override {
            using gkit::test::assert_if;
            ++stopped;
            (void)renderer();
            assert_if(local_target.expired() && local_object.expired(), "queue released before cleanup");
            if (mode != Mode::Draw) return;
            assert_if(reusable->is_uploaded(), "stop inside render still executes the draw queue");
            std::array<GLubyte, 4> left{}, right{};
            const auto screen = window().state();
            glReadBuffer(GL_FRONT);
            glReadPixels(screen.pixel_width / screen.width,
                         screen.pixel_height - 1 - screen.pixel_height / screen.height,
                         1,
                         1,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         left.data());
            assert_if(left[0] == 255 && left[1] == 0 && left[2] == 0, "sprite presented alongside custom draws");
            target->bind();
            glReadPixels(8, 8, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, left.data());
            glReadPixels(24, 8, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, right.data());
            target->unbind();
            assert_if(left == std::array<GLubyte, 4>{255, 0, 0, 255},
                      "first draw keeps the original uniforms and shader");
            assert_if(right == std::array<GLubyte, 4>{0, 255, 0, 255}, "second draw captures the changed uniforms");
        }
    };

    auto test_submission_lifetime() -> bool {
        using Mode = SubmissionProbe::Mode;
        SubmissionProbe app;
        auto check_failure = [&](Mode mode, std::string_view message) {
            app.mode = mode;
            try {
                app.run();
            } catch (const std::runtime_error& error) {
                gkit::test::assert_if(error.what() == message, "original exception propagated");
                return;
            }
            throw std::runtime_error("expected application failure");
        };
        app.run();
        check_failure(Mode::ThrowOnStart, "startup failure");
        check_failure(Mode::ThrowOnRender, "render failure");
        app.mode = Mode::StopBeforeRender;
        app.run();
        app.mode = Mode::Draw;
        app.run();
        return app.rendered == 3 && app.stopped == 5 && SDL_GL_GetCurrentContext() == nullptr;
    }

    auto test_window_attach_failure_and_retry() -> bool {
        using Window = gkit::graphic::Window;
        using Owner  = gkit::core::UniqueObject;
        class Probe final : public gkit::graphic::Application {
            std::optional<Owner> detached;

        public:
            Probe() {
                root().detach_child(*root().find_child("window"), [&](auto result) {
                    detached = std::move(result.owner);
                });
            }
            auto before_frame() -> void override {
                if (auto id = root().find_child("late")) {
                    id->with<Window>([](auto& node) {
                        gkit::test::assert_if(node.state().has_value(), "retry opens the existing window node");
                    });
                    stop();
                } else {
                    gkit::test::assert_if(!SDL_GL_GetCurrentContext(), "start without a window");
                    root().add_child("late", std::move(*detached));
                    detached.reset();
                }
            }
        };
        Probe app;
        app.set_platform_options({.video = false}).set_target_frame_rate(0);
        try {
            app.run();
            return false;
        } catch (const std::invalid_argument&) {
            auto id = app.root().find_child("late");
            if (!id || !id->with<Window>([](auto& node) {
                    gkit::test::assert_if(!node.state(), "failed opening releases the session");
                }))
                return false;
        }
        app.set_platform_options({.video = true});
        app.run();
        return !SDL_GL_GetCurrentContext();
    }

    auto test_sprite_coordinates() -> bool {
        auto projection  = gkit::graphic::SpritePass::logical_projection(320, 180);
        auto clip        = projection * gkit::math::Vector3(40, 30, 1);
        const auto left  = gkit::graphic::SpritePass::pixel_clip({0, 1, 3, 3}, 10, 10, 15, 15);
        const auto right = gkit::graphic::SpritePass::pixel_clip({3, 1, 7, 3}, 10, 10, 15, 15);
        gkit::test::assert_if(left.x == 0 && left.width == 5 && left.y == 9 && left.height == 4 && right.x == 5 &&
                                  right.width == 10,
                              "adjacent clips share the rounded 1.5x pixel edge");
        // A 640 x 360 drawable presents the logical sprite corner at (80, 60) from the top left.
        return std::abs((clip.x + 1.f) * 320.f - 80.f) < 0.001f && std::abs((1.f - clip.y) * 180.f - 60.f) < 0.001f;
    }

    auto test_game_entry_point() -> bool {
        class Player final : public gkit::scene::Sprite2D {
        public:
            Player() {
                set_position({5, 0});
                set_texture(fs::path(__FILE__).parent_path() / "sprite_rows.png");
            }
            auto ready() -> void override {
                SDL_Event event{};
                event.type         = SDL_EVENT_KEY_DOWN;
                event.key.scancode = SDL_SCANCODE_RIGHT;
                SDL_PushEvent(&event);
            }
            auto process(double) -> void override {
                if (gkit::Input::instance().is_key_pressed(gkit::input::Key::Right)) {
                    set_position({get_position().x + 1.f, 0});
                    SDL_Event event{};
                    event.type = SDL_EVENT_QUIT;
                    SDL_PushEvent(&event);
                }
                if (++frames > 3) throw std::runtime_error("input did not reach the player");
            }

        private:
            int frames = 0;
        };
        gkit::graphic::Application app;
        app.set_window_size(64, 32).set_window_title("Player input").set_target_frame_rate(0);
        auto player = gkit::core::UniqueObject::create<Player>();
        auto id     = player.get_id();
        auto level  = gkit::core::UniqueObject::create<gkit::scene::Scene>();
        level.get_id().with<gkit::scene::Scene>([&](auto& scene) {
            scene.add_child("player", std::move(player));
        });
        app.set_main_scene(std::move(level));
        app.run();
        return id.with<Player>([](auto& node) {
            gkit::test::assert_if(node.get_position().x == 6, "player moves after input without a render callback");
            auto previous = node.get_texture();
            try {
                node.set_texture(fs::path(__FILE__).parent_path() / "missing.png");
            } catch (const std::runtime_error& error) {
                gkit::test::assert_if(std::string_view(error.what()).find("missing.png") != std::string_view::npos &&
                                          node.get_texture() == previous,
                                      "failed texture replacement preserves the image");
                return;
            }
            throw std::runtime_error("expected missing texture to fail");
        });
    }

    class MovingGroup final : public gkit::scene::Object2D {
        auto process(double) -> void override {
            auto p = get_position();
            set_position({p.x + 1.f, p.y});
        }
    };

    // Exercise the public game path without overriding render() or receiving window callbacks.
    class SpriteProbe final : public gkit::graphic::Application {
        using Owner  = gkit::core::UniqueObject;
        using Window = gkit::graphic::Window;
        using Sprite = gkit::scene::Sprite2D;

        gkit::core::ObjectId container, initial;
        std::optional<Owner> detached;
        SDL_WindowID first_native{};

        auto pixel(int x, int y) -> std::array<unsigned char, 4> {
            const auto s = window().state();
            std::array<unsigned char, 4> rgba{};
            glReadBuffer(GL_FRONT);
            glReadPixels(x * s.pixel_width / s.width,
                         s.pixel_height - 1 - y * s.pixel_height / s.height,
                         1,
                         1,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         rgba.data());
            return rgba;
        }

        auto check_picture(int x) -> void {
            const auto top     = pixel(x, 5);
            const auto overlap = pixel(x, 7);
            const auto bottom  = pixel(x, 9);
            gkit::test::assert_if(top[0] == 255 && top[2] == 0, "sprite top row and inherited transform");
            gkit::test::assert_if(std::abs(overlap[0] - 127) <= 1 && std::abs(overlap[2] - 128) <= 1,
                                  "later sprite blends over earlier sibling");
            gkit::test::assert_if(bottom[0] == 0 && std::abs(bottom[2] - 128) <= 1, "sprite bottom row");
            gkit::test::assert_if(pixel(x - 1, 7)[0] == 0, "sprite edge moves with its parent");
        }

        auto after_frame() -> void override {
            using gkit::test::assert_if;
            switch (get_frame_context().frame) {
            case 0:
                first_native = window().state().id;
                initial.with<Window>([&](auto& node) {
                    node.reparent(container);
                });
                root().add_child("extra", Owner::create<Window>());
                break;
            case 1: {
                assert_if(window().state().id == first_native, "same-tree move keeps the main window session");
                const auto extra = root().find_child("extra");
                assert_if(extra && extra->with<Window>([](auto& node) {
                    assert_if(!node.state(), "only the designated window opens a session");
                }),
                          "extra window belongs to the same tree");
                check_picture(5);
                container.with<gkit::core::Node>([&](auto& parent) {
                    parent.detach_child(initial, [&](auto result) {
                        detached = std::move(result.owner);
                    });
                });
                break;
            }
            case 2:
                assert_if(detached && !SDL_GL_GetCurrentContext(), "detaching releases the window");
                initial.with<Window>([](auto& node) {
                    assert_if(!node.state(), "detached window state cleared");
                });
                set_main_scene(Owner::create<gkit::scene::Scene>(), [](auto result) {
                    assert_if(result.error == gkit::core::NodeError::Expired && result.owner.has_value(),
                              "return the submitted scene while the main window is detached");
                });
                root().add_child("window", std::move(*detached));
                break;
            case 3:
                assert_if(window().state().id != first_native, "reattached window has a fresh session");
                break;
            case 4: {
                check_picture(7);
                SDL_Event event{};
                event.type            = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
                event.window.windowID = window().state().id;
                SDL_PushEvent(&event);
                break;
            }
            case 5:
                assert_if(!SDL_GL_GetCurrentContext() && root().find_child("window") == initial,
                          "closing keeps simulation running");
                initial.with<Window>([](auto& node) {
                    assert_if(!node.state(), "closed native state");
                });
                open_main_window();
                open_main_window();
                break;
            case 7:
                check_picture(10);
                stop();
                break;
            }
        }

    public:
        SpriteProbe() {
            set_window_title("Automatic sprites").set_window_size(64, 32);
            set_target_frame_rate(0).set_parallel_update_enabled(true).set_worker_count(2);
            initial     = *root().find_child("window");
            auto parent = Owner::create<MovingGroup>();
            auto host   = Owner::create<gkit::core::Node>();
            container   = host.get_id();
            root().add_child("host", std::move(host));
            parent.get_id().with<MovingGroup>([](auto& node) {
                node.set_position({4, 4});
                node.set_scale({2, 2});
                node.set_parallel(true);
                auto frozen = Owner::create<gkit::scene::Scene>();
                frozen.get_id().with<gkit::scene::Scene>([](auto& scene) {
                    scene.pause();
                    for (int y : {1, 0}) {
                        auto sprite = Owner::create<Sprite>();
                        sprite.get_id().with<Sprite>([&](auto& image) {
                            image.set_texture(fs::path(__FILE__).parent_path() / "sprite_rows.png");
                            image.set_position({0, float(y)});
                        });
                        scene.add_child(std::to_string(y), std::move(sprite));
                    }
                });
                node.add_child("paused", std::move(frozen));
            });
            auto level = Owner::create<gkit::scene::Scene>();
            level.get_id().with<gkit::scene::Scene>([&](auto& scene) {
                scene.add_child("moving", std::move(parent));
            });
            set_main_scene(std::move(level));
        }
    };

    using Owner = gkit::core::UniqueObject;

    auto image_at(int x, int y, int scale) -> Owner {
        auto sprite = Owner::create<gkit::scene::Sprite2D>();
        sprite.get_id().with<gkit::scene::Sprite2D>([&](auto& node) {
            node.set_texture(fs::path(__FILE__).parent_path() / "sprite_rows.png");
            node.set_position({float(x), float(y)});
            node.set_scale({float(scale), float(scale)});
        });
        return sprite;
    }

    class Minimap final : public gkit::graphic::Viewport {
    public:
        Minimap() {
            set_rect(18, 5, 6, 6);
            auto inner = Owner::create<gkit::graphic::Viewport>();
            inner.get_id().with<gkit::graphic::Viewport>([](auto& view) {
                view.set_rect(-2, 2, 6, 8);
                view.add_child("tiles", image_at(0, 0, 6));
            });
            add_child("inner", std::move(inner));
        }
    };

    class ViewportLevel final : public gkit::scene::Scene {
    public:
        ViewportLevel() {
            add_child("minimap", Owner::create<Minimap>());
            add_child("overlay", image_at(21, 7, 1));
            add_child("hud", image_at(3, 20, 1));
        }
    };

    class SceneProbe final : public gkit::graphic::Application {
        class RecordedScene final : public gkit::scene::Scene {
        public:
            std::vector<int>* events;
            int tag;
            auto enter_tree() -> void override { events->push_back(tag); }
            auto exit_tree() -> void override { events->push_back(-tag); }
        };
        std::vector<int> events;
        gkit::core::ObjectId final_scene;
        bool cancelled = false;
        auto recorded(int tag) -> Owner {
            auto scene = Owner::create<RecordedScene>();
            scene.get_id().with<RecordedScene>([&](auto& node) {
                node.tag    = tag;
                node.events = &events;
            });
            return scene;
        }
        auto check_pixel(int x, int y, int red, int blue) -> void {
            const auto s = window().state();
            std::array<unsigned char, 4> rgba{};
            glReadBuffer(GL_FRONT);
            glReadPixels(x * s.pixel_width / s.width,
                         s.pixel_height - 1 - y * s.pixel_height / s.height,
                         1,
                         1,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         rgba.data());
            gkit::test::assert_if(std::abs(rgba[0] - red) <= 1 && std::abs(rgba[2] - blue) <= 1,
                                  "viewport pixel at " + std::to_string(x) + "," + std::to_string(y));
        }
        auto after_frame() -> void override {
            using gkit::test::assert_if;
            if (get_frame_context().frame == 1) {
                check_pixel(17, 7, 0, 0); // clipped by the outer viewport
                check_pixel(18, 7, 255, 0);
                check_pixel(20, 10, 255, 0);
                check_pixel(20, 11, 0, 0); // nested clip ends at its parent's bottom edge
                check_pixel(23, 7, 0, 0); // clipped by the inner viewport
                check_pixel(21, 8, 127, 128); // later sibling overlays the viewport
                check_pixel(3, 20, 255, 0); // restored parent clip
                check_pixel(40, 10, 0, 0); // outside the window subtree
                set_main_scene(recorded(1));
                auto next   = recorded(2);
                final_scene = next.get_id();
                set_main_scene(std::move(next));
            }
            if (get_frame_context().frame == 2) {
                assert_if(events == std::vector<int>{1, -1, 2}, "scene replacement exits before entering");
                root().find_child("window")->with<gkit::graphic::Window>([&](auto& main) {
                    assert_if(main.find_child("renamed") == final_scene, "replace renamed scene by identity");
                    assert_if(main.find_child("independent").has_value(), "preserve sibling");
                });
                set_main_scene(recorded(3), [&](auto result) {
                    cancelled = result.error == gkit::core::NodeError::Stopped && result.owner.has_value();
                });
                stop();
            }
        }

    public:
        SceneProbe() {
            set_window_size(64, 32).set_target_frame_rate(0);
            auto scene = Owner::create<ViewportLevel>();
            auto id    = scene.get_id();
            set_main_scene(std::move(scene));
            id.with<ViewportLevel>([](auto& node) {
                node.rename("renamed");
            });
            root().add_child("outside", image_at(40, 10, 1));
            root().find_child("window")->with<gkit::graphic::Window>([](auto& main) {
                main.add_child("independent", Owner::create<gkit::core::Node>());
            });
            bool rejected = false;
            set_main_scene(Owner::create<gkit::core::Node>(), [&](auto result) {
                rejected = result.error == gkit::core::NodeError::InvalidEdit && result.owner.has_value();
            });
            gkit::test::assert_if(rejected, "return invalid owner");
        }
        auto passed() const -> bool { return cancelled && events == std::vector<int>{1, -1, 2, -2}; }
    };

    class MinimizeProbe final : public gkit::graphic::Application {
        bool observed_minimized     = false;
        bool supported              = true;
        bool rendered_after_restore = false;

        auto after_frame() -> void override {
            const auto frame = get_frame_context().frame;
            if (frame >= 120) throw std::runtime_error("window minimize or restore event timed out");
            const auto state = window().state();
            auto* native     = SDL_GetWindowFromID(state.id);
            if (frame == 0) {
                supported = SDL_MinimizeWindow(native);
                if (!supported) {
                    std::cout << "Native minimize/restore unavailable on this SDL video driver\n";
                    stop();
                }
            } else if (state.minimized && !observed_minimized) {
                observed_minimized = true;
            } else if (state.minimized) {
                if (!SDL_RestoreWindow(native)) throw std::runtime_error(SDL_GetError());
            }
        }
        auto render() -> void override {
            gkit::test::assert_if(window().state().drawable(), "skip render while minimized");
            if (observed_minimized) {
                rendered_after_restore = true;
                stop();
            }
        }

    public:
        auto passed() const -> bool { return supported ? rendered_after_restore : !observed_minimized; }
    };

    auto test_minimized_window() -> bool {
        const gkit::platform::WindowState minimized{.pixel_width = 800, .pixel_height = 600, .minimized = true};
        gkit::test::assert_if(!minimized.drawable(), "minimized window has no drawable frame");
        MinimizeProbe app;
        app.run();
        return app.passed();
    }
} // namespace

auto test_render_loop(bool smoke_test) -> bool {
    Demo app(smoke_test);
    app.run();
    return true;
}

auto test_material_helpers() -> bool {
    using gkit::test::assert_if;
    auto texture                            = std::make_shared<TestTexture>();
    std::weak_ptr<TestTexture> weak_texture = texture;
    gkit::graphic::Material material;
    assert_if(!material.is_valid(), "default material requires a shader");
    assert_if(!material.set_texture({}), "empty texture rejected");
    for (std::uint32_t slot = 0; slot < gkit::graphic::MAX_TEXTURE_SLOTS; ++slot) {
        assert_if(material.set_texture(texture), "append texture");
        assert_if(material.get_texture(slot) == texture.get(), "texture slot retains attachment");
    }
    assert_if(!material.set_texture(texture), "full material rejects another texture");
    assert_if(!material.get_texture(material.get_texture_count()), "out-of-range slot");
    auto copy = material;
    texture.reset();
    material = {};
    assert_if(!weak_texture.expired(), "copied material owns textures");
    copy = {};
    assert_if(weak_texture.expired(), "texture released with last material");
    return true;
}

auto main(int argc, char** argv) -> int {
    bool smoke_test  = argc == 2 && std::string_view(argv[1]) == "--smoke-test";
    auto test_runner = gkit::test::TestRunner().add_test_func(test_material_helpers).add_test_func([=] {
        return test_render_loop(smoke_test);
    });
    if (smoke_test)
        test_runner.add_test_func(test_sprite_coordinates)
            .add_test_func(test_game_entry_point)
            .add_test_func(test_window_attach_failure_and_retry)
            .add_test_func(test_submission_lifetime)
            .add_test_func(test_minimized_window)
            .add_test_func([] {
                SceneProbe app;
                app.run();
                return app.passed();
            })
            .add_test_func([] {
                SpriteProbe app;
                app.run();
                return true;
            });

    test_runner.run();
    return 0;
}
