#pragma once
#include <SDL3/SDL_video.h>
#include <string>

namespace core
{

    class Window
    {
    public:
        explicit Window(const std::string& title, int w, int h, SDL_WindowFlags flags);
        ~Window();

        [[nodiscard]] SDL_Window* get_sdl_window_ptr() const noexcept { return window_; }
        [[nodiscard]] uint32_t    get_id() const noexcept;
        void                      get_size(int& width, int& height) const noexcept;

        [[nodiscard]] SDL_WindowFlags get_flags() const noexcept { return flags_; }

    private:
        SDL_Window*     window_ = nullptr;
        SDL_WindowFlags flags_;
    };

} // namespace core
