#pragma once

// internal
#include "Generics.h"

// std
#include <cmath>

namespace Isonia::Math
{
    // Constants
    static constexpr const float PI = 3.14159265359f;
    static constexpr const float TwoPI = PI * 2.0f;

    // 
    static inline float Radians(const float degrees) { return degrees * (PI / 180.0f); }
    static inline float Degrees(const float radians) { return radians * (180.0f / PI); }

    static inline float ClampDegrees(const float degrees) { return Mod(degrees + 360.0f, 360.0f); }
    static inline float ClampRadians(const float radians) { return Mod(radians + TwoPI, TwoPI); }

    static inline float Cos(const float radians) { return std::cos(radians); }
    static inline float Sin(const float radians) { return std::sin(radians); }
    static inline float Atan(const float radians) { return std::atan(radians); }
    static inline float Atan2(const float y, const float x) { return std::atan2(y, x); }
}
