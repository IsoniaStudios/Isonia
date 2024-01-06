#include "Event.h"

namespace Isonia::ECS
{
    Event::Event(EventId type) : mType(type)
    {
    }

    template<typename T>
    void Event::SetParam(EventId id, T value)
    {
        mData[id] = value;
    }

    template<typename T>
    T Event::GetParam(EventId id)
    {
        return std::any_cast<T>(mData[id]);
    }

    EventId Event::GetType() const
    {
        return mType;
    }
}
