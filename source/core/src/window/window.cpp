#include "window.h"
#include <SDL3/SDL_timer.h>
namespace core
{
    Window::Window()
    {
        SDL_WindowFlags window_flags =
            (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
        window_ = SDL_CreateWindow("app", 1280, 720, window_flags);
        if (window_ == nullptr)
        {
            printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        }
    }

    core::Window::~Window() {}

    float core::Window::GetDeltaTime() { return 0.0f; }

    uint64_t core::Window::GetTicks() { return SDL_GetTicks(); }

} // namespace core
