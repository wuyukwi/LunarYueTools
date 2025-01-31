#pragma once

#include <any>
#include <cassert>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace core
{
    struct subsystem_context;

    namespace details
    {
        enum class internal_status : uint8_t
        {
            idle,
            running,
            disposed
        };

        bool               initialize();
        internal_status&   status();
        void               dispose();
        subsystem_context& context();
    } // namespace details

    struct subsystem_context
    {
        bool initialize();
        void dispose();

        template<typename S, typename... Args>
        S& add_subsystem(Args&&... args);

        template<typename S>
        S& get_subsystem();

        template<typename S>
        void remove_subsystem();

        template<typename S>
        bool has_subsystem() const;

        template<typename S1, typename S2, typename... Args>
        bool has_subsystem() const;

    private:
        std::vector<std::type_index>                  _orders;
        std::unordered_map<std::type_index, std::any> _subsystems;
    };

    template<typename S, typename... Args>
    S& subsystem_context::add_subsystem(Args&&... args)
    {
        std::type_index index(typeid(S));
        assert(!has_subsystem<S>() && "Subsystem already exists!");

        _orders.push_back(index);
        _subsystems[index] = std::make_shared<S>(std::forward<Args>(args)...);

        return get_subsystem<S>();
    }

    template<typename S>
    S& subsystem_context::get_subsystem()
    {
        assert(has_subsystem<S>() && "Subsystem not found!");
        return *std::any_cast<std::shared_ptr<S>>(_subsystems[typeid(S)]);
    }

    template<typename S>
    void subsystem_context::remove_subsystem()
    {
        std::type_index index(typeid(S));
        assert(has_subsystem<S>() && "Subsystem not found!");

        _subsystems.erase(index);
        _orders.erase(std::remove(_orders.begin(), _orders.end(), index), _orders.end());
    }

    template<typename S>
    bool subsystem_context::has_subsystem() const
    {
        return _subsystems.find(std::type_index(typeid(S))) != _subsystems.end();
    }

    template<typename S1, typename S2, typename... Args>
    bool subsystem_context::has_subsystem() const
    {
        return has_subsystem<S1>() && has_subsystem<S2, Args...>();
    }

    template<typename S>
    S& get_subsystem()
    {
        assert(details::status() == details::internal_status::running && "Context must be initialized");
        return details::context().get_subsystem<S>();
    }

    template<typename S, typename... Args>
    S& add_subsystem(Args&&... args)
    {
        assert(details::status() == details::internal_status::running && "Context must be initialized");
        return details::context().add_subsystem<S>(std::forward<Args>(args)...);
    }

    template<typename S>
    void remove_subsystem()
    {
        if (details::status() == details::internal_status::running)
        {
            details::context().remove_subsystem<S>();
        }
    }

    template<typename... Args>
    bool has_subsystem()
    {
        return details::status() == details::internal_status::running && details::context().has_subsystem<Args...>();
    }
} // namespace core
