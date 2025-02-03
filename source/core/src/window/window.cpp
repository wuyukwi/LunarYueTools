#include "window.h"
#include "logger.h"
#include <SDL3/SDL_timer.h>
namespace core
{
    Window::Window(const std::string& title, int w, int h, SDL_WindowFlags flags)
    {
        SDL_WindowFlags window_flags =
            (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
        window_ = SDL_CreateWindow(title.c_str(), w, h, flags);
        if (window_ == nullptr)
        {
            APPLOG_ERROR("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        }
    }

    core::Window::~Window()
    {
        if (window_)
        {
            SDL_DestroyWindow(window_);
        }
    }

    uint32_t Window::get_id() { return SDL_GetWindowID(window_); }

    void            Window::get_size(int& width, int& height) const { SDL_GetWindowSize(window_, &width, &height); }
    SDL_WindowFlags Window::get_flags() { return SDL_GetWindowFlags(window_); }
} // namespace core
