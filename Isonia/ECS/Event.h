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

        explicit Event(EventId type) : type(type)
        {
        }

        EventId GetType() const
        {
            return type;
        }

        template<typename T>
        void SetParam(EventId id, T value)
        {
            data[id] = value;
        }

        template<typename T>
        T GetParam(EventId id)
        {
            return std::any_cast<T>(data[id]);
        }

    private:
        EventId type{};
        std::unordered_map<EventId, std::any> data{};
    };
}
