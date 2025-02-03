#pragma once
#include "system/subsystem.h"
#include <SDL3/SDL_video.h>
#include <string>

namespace core
{
    class Window
    {
    public:
        Window(const std::string& title, int w, int h, SDL_WindowFlags flags);
        ~Window();

        SDL_Window* get_sdl_window_ptr() { return window_; }
        uint32_t    get_id();

        void get_size(int& width, int& height) const;

        SDL_WindowFlags get_flags();

    private:
        SDL_Window* window_;
    };
} // namespace core