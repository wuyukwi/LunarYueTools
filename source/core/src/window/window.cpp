#include "window.h"
#include <SDL3/SDL_video.h>
#include <logger.h>
#include <stdexcept>

namespace core
{

    Window::Window(const std::string& title, int w, int h, SDL_WindowFlags flags) : flags_(flags)
    {
        window_ = SDL_CreateWindow(title.c_str(), w, h, flags_);
        if (!window_)
        {
            APPLOG_ERROR(std::string("Failed to create SDL window: ") + SDL_GetError());
        }
    }

    Window::~Window()
    {
        if (window_)
        {
            SDL_DestroyWindow(window_);
            window_ = nullptr;
        }
    }

    uint32_t Window::get_id() const noexcept { return window_ ? SDL_GetWindowID(window_) : 0; }

    void Window::get_size(int& width, int& height) const noexcept
    {
        if (window_)
        {
            SDL_GetWindowSize(window_, &width, &height);
        }
    }

} // namespace core
