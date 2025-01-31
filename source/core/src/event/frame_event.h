#pragma once

#include "event/event_bus.hpp"

struct FrameBegin
{
    float delta_time;
};

struct FrameUpdate
{
    float delta_time;
};

struct FrameRender
{
    float delta_time;
};

struct FrameUiRender
{
    float delta_time;
};

struct FrameEnd
{
    float delta_time;
};
