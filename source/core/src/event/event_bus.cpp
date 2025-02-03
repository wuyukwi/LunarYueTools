#include "event/event_bus.h"

namespace core
{
    void EventBus::unsubscribe(EventHandle handle)
    {
        for (auto& [type, listeners] : _subscribers)
        {
            listeners.erase(
                std::remove_if(listeners.begin(), listeners.end(), [handle](const EventListener& listener) { return listener.id == handle; }),
                listeners.end());
        }
    }

    void EventBus::clear() { _subscribers.clear(); }
} // namespace core
