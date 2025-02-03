#pragma once
#include "system/subsystem.h"

namespace core
{
    class Gui
    {
    public:
        Gui();
        ~Gui();
        void ui_renderer(float dt);
    };
} // namespace core
