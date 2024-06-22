// internal
#include "Math.h"

// std
#include <cmath>

namespace Isonia::Math
{
    extern inline constexpr float radiansf(const float degrees)
    {
        return degrees * (pi / 180.0f);
    }
    extern inline constexpr float degreesf(const float radians)
    {
        return radians * (180.0f / pi);
    }

    extern inline float clampDegreesf(const float degrees)
    {
        return fmodf(degrees + 360.0f, 360.0f);
    }
    extern inline float clampRadiansf(const float radians)
    {
        return fmodf(radians + two_pi, two_pi);
    }

    extern inline float cosf(const float radians)
    {
        return std::cos(radians);
    }
    extern inline float sinf(const float radians)
    {
        return std::sin(radians);
    }
    extern inline float tanf(const float radians)
    {
        return std::tan(radians);
    }
    extern inline float atanf(const float radians)
    {
        return std::atan(radians);
    }
    extern inline float atan2f(const float y, const float x)
    {
        return std::atan2(y, x);
    }
}
