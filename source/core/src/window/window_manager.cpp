#include "window_manager.h"
#include <logger.h>

namespace core
{

    WindowManager::WindowManager(const std::string& title, int w, int h)
    {
        if (SDL_InitSubSystem(SDL_INIT_VIDEO) == false)
        {
            APPLOG_ERROR("SDL_Init failed: {}", SDL_GetError());
        }

        SDL_WindowFlags flags =
            static_cast<SDL_WindowFlags>(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);

        auto main_window = std::make_unique<Window>(title, w, h, flags);
        main_window_id_  = main_window->get_id();
        windows_.emplace(main_window_id_, std::move(main_window));
    }

    WindowManager::~WindowManager()
    {
        windows_.clear();
        SDL_Quit();
    }

    Window* WindowManager::get_main_window() const noexcept
    {
        return windows_.begin()->second.get();
        // auto it = windows_.find(main_window_id_);
        // return it != windows_.end() ? it->second.get() : nullptr;
    }

    Window* WindowManager::get_window_by_id(uint64_t id) const noexcept
    {
        auto it = windows_.find(id);
        return it != windows_.end() ? it->second.get() : nullptr;
    }

    uint32_t WindowManager::get_id() const noexcept
    {
        if (auto* win = get_main_window())
        {
            return SDL_GetWindowID(win->get_sdl_window_ptr());
        }
        return 0;
    }

    void WindowManager::get_size(int& width, int& height) const noexcept
    {
        if (auto* win = get_main_window())
        {
            win->get_size(width, height);
        }
    }

} // namespace core
