#include "app.h"
#include "cmd_line/parser.hpp"
#include "logger.h"
#include "system/subsystem.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <event/EventManager.h>
#include <event/frame_event.h>
#include <event/sdl_event.h>
#include <gui/gui.h>
#include <renderer/renderer.h>
#include <renderer/vulkan/vulkan_renderer.h>
#include <window/window_manager.h>

namespace core
{
    int App::run(const std::string& title, int w, int h, int argc, char* argv[])
    {
        details::initialize();
        Parser::instance(argc, argv);
        setup();
        APPLOG_INFO("App {} setup", title);

        add_subsystem<WindowManager>(title, w, h);
        start();
        APPLOG_INFO("App {} start", title);

        APPLOG_INFO(Parser::instance().getOptionsString());

        auto& window_manager = get_subsystem<WindowManager>();

        auto lastTime = window_manager.get_ticks();
        while (running_)
        {
            auto  currentTime = window_manager.get_ticks();
            float deltaTime   = (currentTime - lastTime) / 1000.0f;
            run_one_frame(deltaTime);
            lastTime = currentTime;
        }

        stop();

        details::dispose();
        return 0;
    }

    void core::App::setup() { Logger::init(true, "log/logs.txt", spdlog::level::info); }

    void core::App::start()
    {
        add_subsystem<EventManager>();
        add_subsystem<VulkanRenderer>();
        add_subsystem<Gui>();
    }

    void core::App::run_one_frame(float dt)
    {
        SDL_Event event;
        auto      window        = get_subsystem<WindowManager>().get_main_window();
        auto&     event_manager = get_subsystem<EventManager>();

        while (SDL_PollEvent(&event))
        {
            event_manager.trigger<SDLEvent>(SDLEvent {&event});
            if (event.type == SDL_EVENT_QUIT)
                running_ = false;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == window->get_id())
                running_ = false;
        }
        if (window->get_flags() & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            return;
        }

        event_manager.trigger<FrameBegin>(FrameBegin {dt});
        event_manager.trigger<FrameUpdate>(FrameUpdate {dt});
        event_manager.trigger<FrameUiRender>(FrameUiRender {dt});
        event_manager.trigger<FrameRender>(FrameRender {dt});
        event_manager.trigger<FrameEnd>(FrameEnd {dt});
    }

    void App::stop() {}
} // namespace core
