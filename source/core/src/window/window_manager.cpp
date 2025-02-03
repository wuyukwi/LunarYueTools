#include "window_manager.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
namespace core
{
    core::WindowManager::WindowManager()
    {
        SDL_WindowFlags window_flags =
            (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
        auto main_window = std::make_unique<Window>("mainWindow", 1280, 720, window_flags);
        auto id          = main_window->get_id();
        windows_.emplace(id, std::move(main_window));
    }

    core::WindowManager::~WindowManager() { SDL_Quit(); }

    uint64_t WindowManager::get_ticks() { return SDL_GetTicks(); }
    Window*  WindowManager::get_main_window() { return windows_.begin()->second.get(); }
    Window*  WindowManager::get_window_by_id(uint64_t id)
    {
        auto it = windows_.find(id);
        if (it != windows_.end())
        {
            return it->second.get();
        }
        return nullptr;
    }
} // namespace core
