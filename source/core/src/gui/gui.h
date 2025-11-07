#pragma once
#include "event/EventSubscriber.h"
#include <event/frame_event.h>

namespace core
{
    class Gui : public EventSubscriber
    {
    public:
        Gui();
        ~Gui();
        void ui_renderer(const FrameUiRender& event);
    };
} // namespace core
