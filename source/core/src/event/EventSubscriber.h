#pragma once
#include "event/EventManager.h"
#include "system/subsystem.h"

namespace core
{
    class EventSubscriber
    {
    protected:
        template<typename Event, typename Class, void (Class::*Method)(const Event&)>
        void connect(Class& instance)
        {
            get_subsystem<EventManager>().connect<Event>(instance, Method);
        }

        template<typename Class>
        void disconnect(Class& instance)
        {
            get_subsystem<EventManager>().disconnect(instance);
        }
    };
} // namespace core
