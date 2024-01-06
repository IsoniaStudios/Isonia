#pragma once

#include "Event.h"
#include "Definitions.h"
#include <functional>
#include <list>
#include <unordered_map>

namespace Isonia::ECS
{
    class EventManager
    {
    public:
        void AddListener(EventId eventId, std::function<void(Event&)> const& listener);

        void SendEvent(Event& event);
        void SendEvent(EventId eventId);

    private:
        std::unordered_map<EventId, std::list<std::function<void(Event&)>>> listeners;
    };
}
