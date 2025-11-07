#pragma once
#include "window.h"
#include <SDL3/SDL.h>
#include <memory>
#include <string>
#include <unordered_map>

namespace core
{

    class WindowManager
    {
    public:
        explicit WindowManager(const std::string& title, int w, int h);
        ~WindowManager();

        [[nodiscard]] Window* get_main_window() const noexcept;
        [[nodiscard]] Window* get_window_by_id(uint64_t id) const noexcept;

        [[nodiscard]] uint64_t get_ticks() const noexcept { return SDL_GetTicks(); }
        [[nodiscard]] uint32_t get_id() const noexcept;
        void                   get_size(int& width, int& height) const noexcept;

    private:
        std::unordered_map<uint64_t, std::unique_ptr<Window>> windows_;
        uint64_t                                              main_window_id_ = 0;
    };

} // namespace core
