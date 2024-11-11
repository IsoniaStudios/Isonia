// internal
#include "Math.h"

// external
#include <cmath>

namespace Isonia::Math
{
    extern constexpr float radiansf(const float degrees)
    {
        return degrees * (pi / 180.0f);
    }
    extern constexpr float degreesf(const float radians)
    {
        return radians * (180.0f / pi);
    }

    extern float clampDegreesf(const float degrees)
    {
        return fmodf(degrees + 360.0f, 360.0f);
    }
    extern float clampRadiansf(const float radians)
    {
        return fmodf(radians + two_pi, two_pi);
    }

    extern float cosf(const float radians)
    {
        return std::cos(radians);
    }
    extern float sinf(const float radians)
    {
        return std::sin(radians);
    }
    extern float tanf(const float radians)
    {
        return std::tan(radians);
    }
    extern float atanf(const float radians)
    {
        return std::atan(radians);
    }
    extern float atan2f(const float y, const float x)
    {
        return std::atan2(y, x);
    }
}
