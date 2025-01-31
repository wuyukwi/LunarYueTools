#include "subsystem.h"

namespace core
{
    bool subsystem_context::initialize() { return true; }

    void subsystem_context::dispose()
    {
        for (auto it = _orders.rbegin(); it != _orders.rend(); ++it)
        {
            auto found = _subsystems.find(*it);
            assert(found != _subsystems.end());

            _subsystems.erase(found);
        }

        _orders.clear();
        assert(_subsystems.empty());
    }

    namespace details
    {
        internal_status& status()
        {
            static internal_status s_status = internal_status::idle;
            return s_status;
        }

        void dispose()
        {
            context().dispose();
            status() = internal_status::disposed;
        }

        subsystem_context& context()
        {
            static subsystem_context s_context;
            return s_context;
        }

        bool initialize()
        {
            if (!context().initialize())
            {
                return false;
            }

            status() = internal_status::running;
            return true;
        }
    } // namespace details
} // namespace core
