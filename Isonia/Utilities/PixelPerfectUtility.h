#pragma once

// internal
#include "MathUtility.h"

// std
#include <cmath>

// external
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Isonia::Utilities::PixelPerfectUtility
{
    // Constants
    static constexpr const float Y_SCALE = 1.154700538379252f; //1.0f / std::cos(Math::Radians(-30.0f));

    static constexpr const float PIXELS_PER_UNIT = 16.0f;
    static constexpr const float UNITS_PER_PIXEL = 1.0f / PIXELS_PER_UNIT;

    static constexpr const float ROTATION_GRID = 30.0f / 16.0f;
    static constexpr const float INVERSE_ROTATION_GRID = 1.0f / ROTATION_GRID;

    static inline glm::vec3 RoundToPixel(const glm::vec3 position)
    {
        return {
            std::roundf(position.x * PIXELS_PER_UNIT) * UNITS_PER_PIXEL,
            std::roundf(position.y * PIXELS_PER_UNIT) * UNITS_PER_PIXEL,
            std::roundf(position.z * PIXELS_PER_UNIT) * UNITS_PER_PIXEL
        };
    }
    static inline glm::vec4 RoundToPixel(const glm::vec4 position)
    {
        return {
            std::roundf(position.x * PIXELS_PER_UNIT) * UNITS_PER_PIXEL,
            std::roundf(position.y * PIXELS_PER_UNIT) * UNITS_PER_PIXEL,
            std::roundf(position.z * PIXELS_PER_UNIT) * UNITS_PER_PIXEL,
            position.w
        };
    }

    /*
    static inline glm::quat RoundToRotation(const glm::quat rotation)
    {
        return Quaternion.Euler(RoundToEulerRotation(rotation.eulerAngles));
    }
    static inline glm::quat RoundToRotation(const glm::vec3 eulerRotation)
    {
        return Quaternion.Euler(RoundToEulerRotation(eulerRotation));
    }
    static inline glm::vec3 RoundToEulerRotation(const glm::quat rotation)
    {
        return RoundToEulerRotation(rotation.eulerAngles);
    }
    */

    static inline glm::vec3 RoundToEulerRotation(const glm::vec3 eulerRotation)
    {
        return {
            std::roundf(eulerRotation.x * INVERSE_ROTATION_GRID) * ROTATION_GRID,
            std::roundf(eulerRotation.y * INVERSE_ROTATION_GRID) * ROTATION_GRID,
            std::roundf(eulerRotation.z * INVERSE_ROTATION_GRID) * ROTATION_GRID
        };
    }
}