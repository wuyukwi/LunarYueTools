#pragma once
#include <algorithm>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace core
{
    class EventManager
    {
    public:
        using HandlerId = size_t;

        struct Subscriber
        {
            void*                            owner;
            std::function<void(const void*)> func;
        };

        std::unordered_map<std::type_index, std::vector<Subscriber>> subscribers_;

        template<typename Event, typename Class>
        void connect(Class& instance, void (Class::*method)(const Event&))
        {
            auto& subs = subscribers_[std::type_index(typeid(Event))];

            subs.push_back(Subscriber {&instance, [&instance, method](const void* e) { (instance.*method)(*static_cast<const Event*>(e)); }});
        }

        template<typename Event, typename... Args>
        void trigger(const Args&... args)
        {
            Event e {args...};
            auto  it = subscribers_.find(std::type_index(typeid(Event)));
            if (it == subscribers_.end())
                return;

            for (auto& sub : it->second)
            {
                sub.func(&e);
            }
        }

        template<typename Class>
        void disconnect(Class& instance)
        {
            for (auto& [type, subs] : subscribers_)
            {
                subs.erase(std::remove_if(subs.begin(), subs.end(), [&](const Subscriber& s) { return s.owner == &instance; }), subs.end());
            }
        }

        void clear() { subscribers_.clear(); }
    };
} // namespace core
