#pragma once

// std
#include <cmath>

namespace Isonia::Utilities::Math
{
    // Generics
    static float Min(float a, float b) { return a < b ? a : b; }

    static float Max(float a, float b) { return a > b ? a : b; }

    static float Abs(float f) { return f < 0 ? -f : f; }

    static float Sqrt(float f) { return sqrtf(f); }

    // To-Int Generics
    static int FloorToInt(float f) { return f >= 0 ? (int)f : (int)f - 1; }

    static int RoundToInt(float f) { return f >= 0 ? (int)(f + 0.5f) : (int)(f - 0.5f); }

    // Interpolation
    static float InterpolationHermite(float t) { return t * t * (3 - 2 * t); }

    static float InterpolationQuintic(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

    // Lerp
    static float Lerp(float a, float b, float t) { return a + t * (b - a); }

    static float CubicLerp(float a, float b, float c, float d, float t)
    {
        float p = (d - c) - (a - b);
        return t * t * t * p + t * t * ((a - b) - p) + t * (c - a) + b;
    }
}