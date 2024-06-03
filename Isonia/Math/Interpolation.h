#pragma once

// internal
#include "Vector.h"

// std
#include <cmath>

namespace Isonia::Math
{
    // Constants

    //
    static inline float InterpolationHermite(const float t) { return t * t * (3.0f - 2.0f * t); }

    static inline float InterpolationQuintic(const float t) { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }

    static inline float Lerp(const float a, const float b, const float t) { return a + t * (b - a); }

    static inline Math::Vector3 Lerp(const Math::Vector3 a, const Math::Vector3 b, const float t)
    {
        return Math::Vector3(
            Lerp(a.x, b.x, t),
            Lerp(a.y, b.y, t),
            Lerp(a.z, b.z, t)
        );
    }

    static inline float CubicLerp(const float a, const float b, const float c, const float d, const float t)
    {
        const float p = (d - c) - (a - b);
        return t * t * t * p + t * t * ((a - b) - p) + t * (c - a) + b;
    }

    static inline Math::Vector3 ComputeSmoothNormalFrom4(const float* heightMap, const float dx, const float dz, const uint32_t width, const uint32_t height, const uint32_t z, const uint32_t x)
    {
        const uint32_t middleIndex = z * width + x;
        const float top = heightMap[middleIndex - width];
        const float left = heightMap[middleIndex - 1];
        const float right = heightMap[middleIndex + 1];
        const float bottom = heightMap[middleIndex + width];

        return Math::Normalize(Math::Vector3{
            bottom - top,
            -dx - dz,
            right - left,
        });
    }

    static inline Math::Vector3 ComputeSmoothNormalFrom8(const float* heightMap, const float dx, const float dz, const uint32_t width, const uint32_t height, const uint32_t z, const uint32_t x)
    {
        const uint32_t middleIndex = z * width + x;
        const float top = heightMap[middleIndex - width];
        const float topLeft = heightMap[middleIndex - width - 1];
        const float topRight = heightMap[middleIndex - width + 1];
        const float left = heightMap[middleIndex - 1];
        const float right = heightMap[middleIndex + 1];
        const float bottom = heightMap[middleIndex + width];
        const float bottomLeft = heightMap[middleIndex + width - 1];
        const float bottomRight = heightMap[middleIndex + width + 1];

        return Math::Normalize(Math::Vector3{
            topRight + right + bottomRight - topLeft - left - bottomLeft,
            -dx - dz - dx - dz,
            bottomLeft + bottom + bottomRight - topLeft - top - topRight
        });
    }
}
