#include "gkit/graphic/opengl/StateManager.hpp"
#include "gkit/graphic/opengl/VertexArray.hpp"
#include "gkit/graphic/opengl/IndexBuffer.hpp"
#include "gkit/graphic/opengl/FrameBuffer.hpp"
#include "gkit/graphic/opengl/RenderBuffer.hpp"
#include "gkit/graphic/Shader.hpp"
#include "gkit/graphic/Renderer.hpp"

#include <glad/gl.h>
#include "SDL3/SDL.h"

#include <iostream>
#include <filesystem>

int main(int argc, char* argv[])
{
    // Get executable directory for resource paths
    std::filesystem::path exePath = argv[0];
    // exe at bin/.../test/test_window.exe, go up 4 levels to reach project root
    std::filesystem::path resourceBase = exePath.parent_path().parent_path().parent_path().parent_path() / "test" ;

    #pragma region Init
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Request OpenGL 4.6 Core Profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    int screenWidth = gkit::graphic::opengl::SCR_WIDTH;
    int screenHeight = gkit::graphic::opengl::SCR_HEIGHT;

    SDL_Window* window = SDL_CreateWindow(
        "OpenGL Window",
        screenWidth,
        screenHeight,
        SDL_WINDOW_OPENGL
    );

    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create OpenGL context
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize GLAD
    if (!gladLoadGL(SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Print OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    #pragma endregion

    {
        #pragma region square
        // Triangle vertex data
        float picVertices[] = {
            // positions        // tex coordsc
            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
            0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
            0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f,  0.0f, 1.0f
        };

        // index data
        unsigned int picIndices[] = { 0, 1, 2, 2, 3, 0 };

        gkit::graphic::opengl::VertexArray picVAO;
        gkit::graphic::opengl::buffer::VertexBuffer picVBO(picVertices, sizeof(picVertices));
        gkit::graphic::opengl::buffer::IndexBuffer picIBO(picIndices, 6);

        gkit::graphic::opengl::buffer::VertexBufferLayout picLayout;
        picLayout.push<float>(3);
        picLayout.push<float>(2);
        picVAO.add_buffer(picVBO, picLayout);

        // load shader source
        gkit::graphic::Shader picShader((resourceBase / "graphic" / "texture.shader").string());
        gkit::graphic::opengl::Texture mainTexture((resourceBase / "graphic" / "container2.png").string(), gkit::graphic::opengl::TextureType::TEXTURE_2D);

        // Full-screen quad vertex data (post-processing)
        float quadVertices[] = {
            // positions                    // tex coords
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
            1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f
        };

        unsigned int quadIndices[] = { 0, 1, 2, 2, 3, 0 };

        gkit::graphic::opengl::VertexArray quadVAO;
        gkit::graphic::opengl::buffer::VertexBuffer quadVB(quadVertices, sizeof(quadVertices));
        gkit::graphic::opengl::buffer::IndexBuffer quadIB(quadIndices, 6);

        gkit::graphic::opengl::buffer::VertexBufferLayout quadLayout;
        quadLayout.push<float>(3);
        quadLayout.push<float>(2);
        quadVAO.add_buffer(quadVB, quadLayout);

        // load post-processing shader
        gkit::graphic::Shader postShader((resourceBase / "graphic" / "post_process.shader").string());
        #pragma endregion

        #pragma region framebuffer
        gkit::graphic::opengl::buffer::FrameBuffer fbo(gkit::graphic::opengl::SCR_WIDTH, gkit::graphic::opengl::SCR_HEIGHT);
        gkit::graphic::opengl::Texture fboTexture(" ", gkit::graphic::opengl::TextureType::TEXTURE_FRAMEBUFFER);
        gkit::graphic::opengl::buffer::RenderBuffer rbo(gkit::graphic::opengl::SCR_WIDTH, gkit::graphic::opengl::SCR_HEIGHT);
        fbo.attach_color_texture(fboTexture, 0);
        fbo.attach_depth_stencil(rbo);
        fbo.check();
        #pragma endregion

        auto& renderer = gkit::graphic::Renderer::instance();
        auto& stateManager = gkit::graphic::opengl::StateManager::instance();
        
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

            fbo.bind();
            fbo.set_viewport(0, 0, screenWidth, screenHeight);
            renderer.clear(gkit::graphic::opengl::ClearFlags::All);
            // 1. Render to framebuffer
            picShader.bind();
            mainTexture.bind(0);
            renderer.draw(picVAO, picIBO, picShader);

            // 2. Render to screen (post-processing)
            fbo.unbind();
            renderer.clear(gkit::graphic::opengl::ClearFlags::All);
            gkit::graphic::opengl::viewport::set_viewport(0, 0, screenWidth/2, screenHeight/2);
            stateManager.set_stencil_test(true);
            stateManager.set_stencil(gkit::graphic::opengl::CompareFunc::Always, 1, 0xFF);
            stateManager.set_stencil_op(
                    gkit::graphic::opengl::StencilOp::Keep, 
                gkit::graphic::opengl::StencilOp::Keep, 
                gkit::graphic::opengl::StencilOp::Replace);
            stateManager.apply();
            picShader.bind();
            mainTexture.bind(0);
            renderer.draw(picVAO, picIBO, picShader);

            gkit::graphic::opengl::viewport::set_viewport(0, 0, screenWidth, screenHeight);
            stateManager.set_stencil(gkit::graphic::opengl::CompareFunc::Equal, 1, 0xFF);
            stateManager.set_stencil_op(
                    gkit::graphic::opengl::StencilOp::Keep, 
                gkit::graphic::opengl::StencilOp::Keep, 
                gkit::graphic::opengl::StencilOp::Keep);
            stateManager.apply();
            postShader.bind();
            fboTexture.bind(0);
            postShader.set_uniform_1i("screenTexture", 0);
            renderer.draw(quadVAO, quadIB, postShader);
            
            gkit::graphic::opengl::viewport::set_viewport(0, 0, screenWidth/4, screenHeight/4);
            postShader.bind();
            fboTexture.bind(0);
            postShader.set_uniform_1i("screenTexture", 0);
            renderer.draw(quadVAO, quadIB, postShader);
            
            stateManager.set_stencil_test(false);
            stateManager.apply();

            // Swap buffers
            SDL_GL_SwapWindow(window);
        }
    }

    // Cleanup
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "Window closed successfully" << std::endl;
    return 0;
}
