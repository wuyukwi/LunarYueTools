#pragma once

#include <any>
#include <cassert>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace core
{
    class EventBus
    {
    public:
        static EventBus& instance()
        {
            static EventBus eventBus;
            return eventBus;
        }

        using EventHandle = size_t;

        template<typename EventType, typename Func>
        EventHandle subscribe(Func&& callback);

        template<typename EventType>
        void publish(const EventType& event);

        void unsubscribe(EventHandle handle);
        void clear();

    private:
        struct EventListener
        {
            std::type_index               type;
            size_t                        id;
            std::function<void(std::any)> callback;
        };

        std::unordered_map<std::type_index, std::vector<EventListener>> _subscribers;
        size_t                                                          _nextHandleId = 1;
    };

    template<typename EventType, typename Func>
    EventBus::EventHandle EventBus::subscribe(Func&& callback)
    {
        std::type_index eventType(typeid(EventType));
        EventHandle     handle = _nextHandleId++;

        _subscribers[eventType].push_back({eventType, handle, [cb = std::forward<Func>(callback)](std::any event) {
                                               if constexpr (std::is_invocable_v<Func>)
                                               {
                                                   cb();
                                               }
                                               else
                                               {
                                                   cb(std::any_cast<const EventType&>(event));
                                               }
                                           }});
        return handle;
    }

    template<typename EventType>
    void EventBus::publish(const EventType& event)
    {
        std::type_index eventType(typeid(EventType));
        auto            it = _subscribers.find(eventType);
        if (it != _subscribers.end())
        {
            for (const auto& listener : it->second)
            {
                listener.callback(event);
            }
        }
    }
} // namespace core
