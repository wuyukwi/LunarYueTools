#include "app.h"
#include "cmd_line/parser.hpp"
#include "logger.h"
#include "system/subsystem.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <event/frame_event.h>
#include <event/sdl_event.h>
#include <gui/gui.h>
#include <renderer/renderer.h>
#include <renderer/vulkan/vulkan_renderer.h>
#include <window/window_manager.h>

using namespace core;

int App::run(const std::string& title, int w, int h, int argc, char* argv[])
{
    details::initialize();
    Parser::instance(argc, argv);
    setup();
    APPLOG_INFO("App {} setup", title);

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

void core::App::setup()
{
    Logger::init(true, "log/logs.txt", spdlog::level::info);

    // Setup SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD) != 0)
    {
        APPLOG_ERROR("Error: SDL_Init(): {}", SDL_GetError());
    }
}

void core::App::start()
{
    add_subsystem<WindowManager>();
    add_subsystem<VulkanRenderer>();
    add_subsystem<Gui>();
}

void core::App::run_one_frame(float dt)
{
    auto& bus = EventBus::instance();

    SDL_Event event;
    auto      window = get_subsystem<WindowManager>().get_main_window();

    while (SDL_PollEvent(&event))
    {
        bus.publish<SDLEvent>(SDLEvent {&event});
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

    bus.publish<FrameBegin>(FrameBegin {dt});
    bus.publish<FrameUpdate>(FrameUpdate {dt});
    bus.publish<FrameUiRender>(FrameUiRender {dt});
    bus.publish<FrameRender>(FrameRender {dt});
    bus.publish<FrameEnd>(FrameEnd {dt});
}

void App::stop() {}
