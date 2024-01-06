#include "Event.h"

namespace Isonia::ECS
{
    Event::Event(EventId type) : mType(type)
    {
    }

    EventId Event::GetType() const
    {
        return mType;
    }
}
