#pragma once

// std
#include <cmath>

// external
#include <glm/glm.hpp>

namespace Isonia::Utilities::Math
{
    // Constants
    static constexpr const float PI = 3.14159265359f;

    // 
    static inline float Radians(const float degrees) { return degrees * (PI / 180.0f); }

    static inline float Degrees(const float radians) { return radians * (180.0f / PI); }

    static inline float ClampRotation(const float rotation) { return std::fmodf(rotation + 360.0f, 360.0f); }

    // Generics
    static inline float Min(const float a, const float b) { return a < b ? a : b; }

    static inline float Max(const float a, const float b) { return a > b ? a : b; }

    static inline float Abs(const float f) { return f < 0 ? -f : f; }

    static inline float Sqrt(const float f) { return std::sqrtf(f); }

    // To-Int Generics
    static inline int32_t CeilToInt(const float f) { return f >= 0 ? static_cast<int32_t>(f + 0.999999f) : static_cast<int32_t>(f); }

    static inline int32_t FloorToInt(const float f) { return f >= 0 ? static_cast<int32_t>(f) : static_cast<int32_t>(f) - 1; }

    static inline int32_t RoundToInt(const float f) { return f >= 0 ? static_cast<int32_t>(f + 0.5f) : static_cast<int32_t>(f - 0.5f); }

    static inline int32_t GetCeiledEvenNumber(const float number)
    {
        int32_t ceiledNumber = CeilToInt(number);
        if (ceiledNumber % 2 != 0) // Check if the number is odd
            ceiledNumber++;        // Increment by 1 to make it even
        return ceiledNumber;
    }
    static inline int32_t GetCeiledOddNumber(const float number)
    {
        int32_t ceiledNumber = CeilToInt(number);
        if (ceiledNumber % 2 == 0) // Check if the number is even
            ceiledNumber++;        // Increment by 1 to make it odd
        return ceiledNumber;
    }

    // Interpolation
    static inline float InterpolationHermite(const float t) { return t * t * (3.0f - 2.0f * t); }

    static inline float InterpolationQuintic(const float t) { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }

    // Lerp
    static inline float Lerp(const float a, const float b, const float t) { return a + t * (b - a); }

    static inline glm::vec3 Lerp(const glm::vec3 a, const glm::vec3 b, const float t)
    {
        return glm::vec3(
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

    static inline glm::vec3 ComputeSmoothNormalFrom4(const float* heightMap, const float dx, const float dz, const uint32_t width, const uint32_t height, const uint32_t z, const uint32_t x)
    {
        const uint32_t middleIndex = z * width + x;
        const float top = heightMap[middleIndex - width];
        const float left = heightMap[middleIndex - 1];
        const float right = heightMap[middleIndex + 1];
        const float bottom = heightMap[middleIndex + width];

        return glm::normalize(glm::vec3{
            bottom - top,
            -dx -dz,
            right - left,
        });
    }

    static inline glm::vec3 ComputeSmoothNormalFrom8(const float* heightMap, const float dx, const float dz, const uint32_t width, const uint32_t height, const uint32_t z, const uint32_t x)
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

        return glm::normalize(glm::vec3{
            topRight + right + bottomRight - topLeft - left - bottomLeft,
            -dx -dz -dx -dz,
            bottomLeft + bottom + bottomRight - topLeft - top - topRight
        });
    }
}