#pragma once

// internal
#include "Definitions.h"

// std
#include <any>
#include <unordered_map>

namespace Isonia::ECS
{
    class Event
    {
    public:
        Event() = delete;

        explicit Event(EventId type) : mType(type)
        {
        }

        EventId GetType() const
        {
            return mType;
        }

        template<typename T>
        void SetParam(EventId id, T value)
        {
            mData[id] = value;
        }

        template<typename T>
        T GetParam(EventId id)
        {
            return std::any_cast<T>(mData[id]); // 123123
        }

    private:
        EventId mType{};
        std::unordered_map<EventId, std::any> mData{};
    };
}
