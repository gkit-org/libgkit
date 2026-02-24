#include "gkit.hpp"
#include <SDL3/SDL_video.h>
#include <chrono>
#include <thread>

#include <SDL3/SDL.h>

int main() {
    gkit::Application app;

    gkit::graphic::Window win("Hello World", 640, 480);
    std::this_thread::sleep_for(std::chrono::seconds(3));

    return 0;
}