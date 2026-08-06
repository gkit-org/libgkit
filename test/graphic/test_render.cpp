#include "gkit/graphic/VertexBufferLayout.hpp"
#include "gkit/graphic/config.hpp"
#include "gkit/graphic/render/Renderer.hpp"
#include "graphic/backend/opengl/Texture.hpp"
#include "test_utils.hpp"

#include <filesystem>
#include <iostream>

#include "SDL3/SDL.h"
#include <glad/gl.h>

namespace fs = std::filesystem;

auto test_render_loop() -> bool {
    // Resource files live in <project>/test/graphic/, same folder as this source file
    fs::path resource_base = fs::path(__FILE__).parent_path();

#pragma region Init
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << '\n';
        return false;
    }

    // Request OpenGL 4.6 Core Profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    int screen_width  = gkit::graphic::SCR_WIDTH;
    int screen_height = gkit::graphic::SCR_HEIGHT;

    SDL_Window* window = SDL_CreateWindow("OpenGL Window", screen_width, screen_height, SDL_WINDOW_OPENGL);

    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << '\n';
        SDL_Quit();
        return false;
    }

    // Create OpenGL context
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // Initialize GLAD
    if (!gladLoadGL(SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << '\n';
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // Print OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << '\n';
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';

#pragma endregion

    {
        auto& renderer = gkit::graphic::Renderer::instance();
        renderer.init(); // default OpenGL backend

        auto& device = renderer.get_device();

#pragma region triangle
        // Colored triangle vertex data (position + color)
        std::vector<float> tri_vertices = {// positions            // colors
                                           0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top: red
                                           -0.4f, -0.25f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom-left: green
                                           0.4f, -0.25f,  0.0f, 0.0f, 0.0f, 1.0f}; // bottom-right: blue

        std::vector<uint32_t> tri_indices = {0, 1, 2};

        gkit::graphic::VertexBufferLayout tri_layout;
        tri_layout.push<float>(3); // position
        tri_layout.push<float>(3); // color

        // load shader source
        auto tri_shader = device.create_shader((resource_base / "color_triangle.shader").string());

        // Full-screen quad vertex data (post-processing).
        // z = 0.2 (farthest): it depth-tests first and writes the deepest value,
        // so the later triangles (z = 0.1 and z = 0) depth-test against it.
        std::vector<float> quad_vertices = {// positions                    // tex coords
                                            -1.0f, -1.0f, 0.2f, 0.0f, 0.0f,
                                            1.0f,  -1.0f, 0.2f, 1.0f, 0.0f,
                                            1.0f,  1.0f,  0.2f, 1.0f, 1.0f,
                                            -1.0f, 1.0f,  0.2f, 0.0f, 1.0f};

        std::vector<uint32_t> quad_indices = {0, 1, 2, 2, 3, 0};

        gkit::graphic::VertexBufferLayout quad_layout;
        quad_layout.push<float>(3);
        quad_layout.push<float>(2);

        // load post-processing shader
        auto post_shader = device.create_shader((resource_base / "post_process.shader").string());

        // load alpha-blended triangle shader (u_alpha uniform controls opacity)
        auto alpha_shader = device.create_shader((resource_base / "alpha_triangle.shader").string());
#pragma endregion

#pragma region framebuffer
        // FBO is half the window size.
        const int fbo_width  = screen_width;
        const int fbo_height = screen_height;
        auto fbo             = device.create_frame_buffer(fbo_width, fbo_height);
        gkit::graphic::opengl::Texture fbo_texture(" ", gkit::graphic::TextureType::TextureFramebuffer);
        auto rbo = device.create_render_buffer(fbo_width, fbo_height);
        fbo->attach_color_texture(fbo_texture, 0);
        fbo->attach_depth_stencil(*rbo);
        fbo->check();
#pragma endregion

#pragma region render_objects
        // Reusable draw units: user provides data arrays + material; VAO/VBO/IBO hidden.

        // Triangle material
        gkit::graphic::Material tri_material;
        tri_material.shader = tri_shader.get();

        // Post-processing quad material (samples the FBO texture)
        gkit::graphic::Material post_material;
        post_material.shader        = post_shader.get();
        post_material.textures[0]   = &fbo_texture;
        post_material.texture_count = 1;
        post_material.uniforms.values.push_back({"screenTexture", 0});

        // Opaque objects enable depth testing so they write their depth and the
        // translucent triangle is really depth-tested against them (a disabled
        // depth test does not write the depth buffer, which would leave it empty).
        gkit::graphic::RenderObject triangle_obj(tri_vertices, tri_indices, tri_layout, tri_material);
        triangle_obj.state.depth.enabled = true;

        gkit::graphic::RenderObject quad_obj(quad_vertices, quad_indices, quad_layout, post_material);
        quad_obj.state.depth.enabled = true;

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
        alpha_material.shader = alpha_shader.get();
        alpha_material.uniforms.values.push_back({"u_alpha", 0.8f});

        gkit::graphic::RenderObject alpha_triangle_obj(alpha_vertices, tri_indices, tri_layout, alpha_material);
        alpha_triangle_obj.state.depth.enabled   = true; // depth-tests against quad + comparison triangle
        alpha_triangle_obj.state.blend.enabled   = true;
        alpha_triangle_obj.state.blend.src_rgb   = gkit::graphic::BlendFunc::SrcAlpha;
        alpha_triangle_obj.state.blend.dst_rgb   = gkit::graphic::BlendFunc::OneMinusSrcAlpha;
        alpha_triangle_obj.state.blend.src_alpha = gkit::graphic::BlendFunc::SrcAlpha;
        alpha_triangle_obj.state.blend.dst_alpha = gkit::graphic::BlendFunc::OneMinusSrcAlpha;
        alpha_triangle_obj.transparent           = true; // sorted after opaque, back-to-front
        alpha_triangle_obj.depth_key             = 0.0f; // nearest transparent → drawn last

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
            stencil_vertices.insert(stencil_vertices.end(), tri_vertices.begin() + v + 3, tri_vertices.begin() + v + 6);
        }

        gkit::graphic::RenderObject stencil_triangle_obj(stencil_vertices, tri_indices, tri_layout, tri_material);
        stencil_triangle_obj.state.stencil.enabled      = true;
        stencil_triangle_obj.state.stencil.compare_func = gkit::graphic::CompareFunc::Always; // always pass, just write
        stencil_triangle_obj.state.stencil.ref          = 1;
        stencil_triangle_obj.state.stencil.write_mask   = 0xFF;
        stencil_triangle_obj.state.stencil.fail         = gkit::graphic::StencilOp::Keep;
        stencil_triangle_obj.state.stencil.z_fail       = gkit::graphic::StencilOp::Keep;
        stencil_triangle_obj.state.stencil.z_pass       = gkit::graphic::StencilOp::Replace; // write stencil=ref=1

        // Triangle drawn after the stencil mask: NotEqual(1) rejects fragments
        // inside the mask region, leaving a hole there and drawing elsewhere.
        gkit::graphic::RenderObject masked_triangle_obj(tri_vertices, tri_indices, tri_layout, tri_material);
        masked_triangle_obj.state.depth.enabled       = true;
        masked_triangle_obj.state.stencil.enabled     = true;
        masked_triangle_obj.state.stencil.compare_func = gkit::graphic::CompareFunc::Notequal; // stencil==1 fails
        masked_triangle_obj.state.stencil.ref          = 1;
        masked_triangle_obj.state.stencil.fail         = gkit::graphic::StencilOp::Keep;
        masked_triangle_obj.state.stencil.z_fail       = gkit::graphic::StencilOp::Keep;
        masked_triangle_obj.state.stencil.z_pass       = gkit::graphic::StencilOp::Keep;
#pragma endregion

        // Main loop
        bool quit = false;
        SDL_Event event;
        while (!quit) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT) {
                    quit = true;
                }
                if (event.type == SDL_EVENT_KEY_DOWN) {
                    if (event.key.key == SDLK_ESCAPE) {
                        quit = true;
                    }
                }
            }

            // Clear the default framebuffer (screen) every frame. Depth is cleared
            // too: the translucent triangle depth-tests, so a stale depth buffer
            // would reject it on later frames.
            renderer.clear(gkit::graphic::ClearFlags::ColorDepth);

            // Submit reusable render objects; Renderer enqueues them and flush() executes.
            // FBO stencil-mask pass:
            // Draw 1: clear the FBO (color+depth+stencil) then write stencil=1 in the
            //         mask triangle's region (offset 50 px up; color is irrelevant).
            stencil_triangle_obj.clear = true; // clears color+depth+stencil (All)
            renderer.draw(stencil_triangle_obj, fbo.get());
            // Draw 2: clear only the FBO color (keeps the stencil marks), then draw the
            //         triangle with stencil NotEqual(1): fragments inside the stencil
            //         region are rejected, leaving a hole.
            masked_triangle_obj.clear       = true;
            masked_triangle_obj.clear_flags = gkit::graphic::ClearFlags::Color;
            renderer.draw(masked_triangle_obj, fbo.get());
            // Draw 3: post-processing quad to screen (samples fbo texture)
            renderer.draw(quad_obj);
            // Draw 4:triangle to screen (no post-processing, just for comparison)
            triangle_obj.clear = false;
            renderer.draw(triangle_obj, nullptr, gkit::graphic::Viewport{.x=0, .y=0, .width=screen_width / 2, .height=screen_height / 2});
            // Draw 5: translucent triangle blended over the screen (depth-tested on
            // top of the FBO quad; transparent flag sorts it last, so it's the top layer).
            renderer.draw(alpha_triangle_obj);

            renderer.flush();

            // Swap buffers
            SDL_GL_SwapWindow(window);
        }
    }

    // Cleanup
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    gkit::test::logln("window closed successfully");
    return true;
}

auto main() -> int {
    auto test_runner = gkit::test::TestRunner().add_test_func(test_render_loop);

    test_runner.run();
    return 0;
}
