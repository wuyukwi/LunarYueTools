#pragma once
#include "system/subsystem.h"
#include <SDL3/SDL_video.h>
#include <string>

namespace core
{
    class Window
    {
    public:
        Window();
        ~Window();

        float GetDeltaTime();

        uint64_t GetTicks();

        SDL_Window* GetWindowPtr() { return window_; }

    private:
        SDL_Window* window_;
    };
} // namespace core