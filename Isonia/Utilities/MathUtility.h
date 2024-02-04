#pragma once

// std
#include <cmath>

namespace Isonia::Utilities::Math
{
    // Generics
    static inline float Min(float a, float b) { return a < b ? a : b; }

    static inline float Max(float a, float b) { return a > b ? a : b; }

    static inline float Abs(float f) { return f < 0 ? -f : f; }

    static inline float Sqrt(float f) { return sqrtf(f); }

    // To-Int Generics
    static inline int FloorToInt(float f) { return f >= 0 ? (int)f : (int)f - 1; }

    static inline int RoundToInt(float f) { return f >= 0 ? (int)(f + 0.5f) : (int)(f - 0.5f); }

    // Interpolation
    static inline float InterpolationHermite(float t) { return t * t * (3 - 2 * t); }

    static inline float InterpolationQuintic(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

    // Lerp
    static inline float Lerp(float a, float b, float t) { return a + t * (b - a); }

    static inline float CubicLerp(float a, float b, float c, float d, float t)
    {
        float p = (d - c) - (a - b);
        return t * t * t * p + t * t * ((a - b) - p) + t * (c - a) + b;
    }
}