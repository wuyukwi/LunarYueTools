#pragma once
#include "system/subsystem.h"
#include "window/window.h"
#include <memory>
#include <unordered_map>

namespace core
{
    class WindowManager
    {
    public:
        WindowManager();
        ~WindowManager();

        uint64_t get_ticks();
        Window*  get_main_window();

        Window* get_window_by_id(uint64_t id);

        WindowManager(const WindowManager&)            = delete;
        WindowManager& operator=(const WindowManager&) = delete;

    private:
        std::unordered_map<uint32_t, std::unique_ptr<Window>> windows_;
    };
} // namespace core