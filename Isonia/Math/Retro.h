#pragma once

// internal
#include "Vector.h"
#include "Generics.h"

namespace Isonia::Math::Retro
{
    // Constants
    static constexpr const float Y_SCALE = 1.154700538379252f; //1.0f / Math::Cos(Math::Radians(-30.0f));

    static constexpr const float PIXELS_PER_UNIT = 16.0f;
    static constexpr const float UNITS_PER_PIXEL = 1.0f / PIXELS_PER_UNIT;

    static constexpr const float ROTATION_GRID = 30.0f / 16.0f;
    static constexpr const float INVERSE_ROTATION_GRID = 1.0f / ROTATION_GRID;

    static uint32_t RenderFactor = 1u;

    static inline Math::Vector3 RoundToPixel(const Math::Vector3 position)
    {
        return {
            Math::Round(position.x * PIXELS_PER_UNIT)* UNITS_PER_PIXEL,
            Math::Round(position.y * PIXELS_PER_UNIT) * UNITS_PER_PIXEL,
            Math::Round(position.z * PIXELS_PER_UNIT) * UNITS_PER_PIXEL
        };
    }
    static inline Math::Vector4 RoundToPixel(const Math::Vector4 position)
    {
        return {
            Math::Round(position.x * PIXELS_PER_UNIT) * UNITS_PER_PIXEL,
            Math::Round(position.y * PIXELS_PER_UNIT) * UNITS_PER_PIXEL,
            Math::Round(position.z * PIXELS_PER_UNIT) * UNITS_PER_PIXEL,
            position.w
        };
    }

    /*
    static inline Math::quat RoundToRotation(const Math::quat rotation)
    {
        return Quaternion.Euler(RoundToEulerRotation(rotation.eulerAngles));
    }
    static inline Math::quat RoundToRotation(const Math::Vector3 eulerRotation)
    {
        return Quaternion.Euler(RoundToEulerRotation(eulerRotation));
    }
    static inline Math::Vector3 RoundToEulerRotation(const Math::quat rotation)
    {
        return RoundToEulerRotation(rotation.eulerAngles);
    }
    */

    static inline Math::Vector3 RoundToEulerRotation(const Math::Vector3 eulerRotation)
    {
        return {
            Math::Round(eulerRotation.x * INVERSE_ROTATION_GRID) * ROTATION_GRID,
            Math::Round(eulerRotation.y * INVERSE_ROTATION_GRID) * ROTATION_GRID,
            Math::Round(eulerRotation.z * INVERSE_ROTATION_GRID) * ROTATION_GRID
        };
    }
}
