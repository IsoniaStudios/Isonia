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
    static inline float Radians(float degrees) { return degrees * (PI / 180.0f); }

    static inline float Degrees(float radians) { return radians * (180.0f / PI); }

    static inline float ClampRotation(float rotation) { return std::fmodf(rotation + 360.0f, 360.0f); }

    // Generics
    static inline float Min(float a, float b) { return a < b ? a : b; }

    static inline float Max(float a, float b) { return a > b ? a : b; }

    static inline float Abs(float f) { return f < 0 ? -f : f; }

    static inline float Sqrt(float f) { return sqrtf(f); }

    // To-Int Generics
    static inline int32_t CeilToInt(float f) { return f >= 0 ? (int)(f + 0.999999f) : (int)f; }

    static inline int32_t FloorToInt(float f) { return f >= 0 ? (int)f : (int)f - 1; }

    static inline int32_t RoundToInt(float f) { return f >= 0 ? (int)(f + 0.5f) : (int)(f - 0.5f); }

    static inline int32_t GetCeiledEvenNumber(float number)
    {
        int32_t ceiledNumber = CeilToInt(number);
        if (ceiledNumber % 2 != 0) // Check if the number is odd
            ceiledNumber++;        // Increment by 1 to make it even
        return ceiledNumber;
    }
    static inline int32_t GetCeiledOddNumber(float number)
    {
        int32_t ceiledNumber = CeilToInt(number);
        if (ceiledNumber % 2 == 0) // Check if the number is even
            ceiledNumber++;        // Increment by 1 to make it odd
        return ceiledNumber;
    }

    // Interpolation
    static inline float InterpolationHermite(float t) { return t * t * (3 - 2 * t); }

    static inline float InterpolationQuintic(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

    // Lerp
    static inline float Lerp(float a, float b, float t) { return a + t * (b - a); }

    static inline glm::vec3 Lerp(glm::vec3 a, glm::vec3 b, float t)
    {
        return glm::vec3(
            Lerp(a.x, b.x, t),
            Lerp(a.y, b.y, t),
            Lerp(a.z, b.z, t)
        );
    }

    static inline float CubicLerp(float a, float b, float c, float d, float t)
    {
        float p = (d - c) - (a - b);
        return t * t * t * p + t * t * ((a - b) - p) + t * (c - a) + b;
    }

    // 
    static inline glm::vec3 ComputeSmoothNormalFrom4(const glm::vec3& v01,
                               const glm::vec3& v10, const glm::vec3& v11, const glm::vec3& v12,
                                                     const glm::vec3& v21)
    {
        // Calculate flat normal for neighbouring 4 triangles
        glm::vec3 t0 = glm::cross(v01 - v11, v12 - v11);
        glm::vec3 t1 = glm::cross(v12 - v11, v21 - v11);

        glm::vec3 t2 = glm::cross(v21 - v11, v10 - v11);
        glm::vec3 t3 = glm::cross(v10 - v11, v01 - v11);

        // Compute the smooth normal
        glm::vec3 smoothNormal = glm::normalize(t0 + t1 + t2 + t3);

        return smoothNormal;
    }
}