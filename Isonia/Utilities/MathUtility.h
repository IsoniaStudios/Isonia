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

    static inline float Sqrt(float f) { return std::sqrtf(f); }

    // To-Int Generics
    static inline int32_t CeilToInt(float f) { return f >= 0 ? static_cast<int32_t>(f + 0.999999f) : static_cast<int32_t>(f); }

    static inline int32_t FloorToInt(float f) { return f >= 0 ? static_cast<int32_t>(f) : static_cast<int32_t>(f) - 1; }

    static inline int32_t RoundToInt(float f) { return f >= 0 ? static_cast<int32_t>(f + 0.5f) : static_cast<int32_t>(f - 0.5f); }

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
    static inline float InterpolationHermite(float t) { return t * t * (3.0f - 2.0f * t); }

    static inline float InterpolationQuintic(float t) { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }

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

    static inline glm::vec3 ComputeSmoothNormalFrom4(float* heightMap, float dx, float dz, uint32_t width, uint32_t height, uint32_t z, uint32_t x)
    {
        const uint32_t middleIndex = z * width + x;
        const float top = heightMap[middleIndex - width];
        const float left = heightMap[middleIndex - 1];
        const float middle = heightMap[middleIndex];
        const float right = heightMap[middleIndex + 1];
        const float bottom = heightMap[middleIndex + width];
        return glm::normalize(
            glm::vec3{
                (bottom - middle) + (middle - top),
                -dx -dz,
                (right - middle) + (middle - left),
            }
        );
    }

    static inline glm::vec3 ComputeSmoothNormalFrom9(float* heightMap, uint32_t width, uint32_t height, float scale, float level, uint32_t z, uint32_t x)
    {
        const uint32_t middleIndex = z * width + x;
        const float topLeft =     heightMap[middleIndex - 1 - width];
        const float top =         heightMap[middleIndex - width];
        const float topRight =    heightMap[middleIndex + 1 - width];
        const float left =        heightMap[middleIndex - 1];
        const float right =       heightMap[middleIndex + 1];
        const float bottomLeft =  heightMap[middleIndex - 1 + width];
        const float bottom =      heightMap[middleIndex + width];
        const float bottomRight = heightMap[middleIndex + 1 + width];

        const float dX = topLeft * 3.0f + left * 10.0f + bottomLeft * 3.0f - topRight * 3.0f - right * 10.0f - bottomRight * 3.0f;
        const float dY = topLeft * 3.0f + top * 10.0f + topRight * 3.0f - bottomLeft * 3.0f - bottom * 10.0f - bottomRight * 3.0f;
        const float dZ = 1.0f / scale * (1.0f + pow(2.0f, level));

        const float length = sqrt((dX * dX) + (dY * dY) + dZ * dZ);

        return{
            ((dX / length) * 0.5f + 0.5f),
            ((dY / length) * 0.5f - 0.5f),
            ((dZ / length))
        };
    }
}