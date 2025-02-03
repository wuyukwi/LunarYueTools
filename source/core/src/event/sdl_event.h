#pragma once

#include "event/event_bus.h"
#include <SDL3/SDL_events.h>

namespace core
{
    struct SDLEvent
    {
        SDL_Event* event;
    };
} // namespace core
