#pragma once

// std
#include <cmath>

// external
#include <glm/glm.hpp>

namespace Isonia::Utilities::Math
{
    // Constants
    const float PI = 3.14159265359;

    // 
    static inline float Radians(float degrees) { return degrees * (PI / 180.0f); }

    static inline float Degrees(float radians) { return radians * (180.0f / PI); }

    //
    const float Y_SCALE = 1.0 / std::cos(Radians(-30.0));

    // Generics
    static inline float Min(float a, float b) { return a < b ? a : b; }

    static inline float Max(float a, float b) { return a > b ? a : b; }

    static inline float Abs(float f) { return f < 0 ? -f : f; }

    static inline float Sqrt(float f) { return sqrtf(f); }

    // To-Int Generics
    static inline int CeilToInt(float f) { return f >= 0 ? (int)(f + 0.999999f) : (int)f; }

    static inline int FloorToInt(float f) { return f >= 0 ? (int)f : (int)f - 1; }

    static inline int RoundToInt(float f) { return f >= 0 ? (int)(f + 0.5f) : (int)(f - 0.5f); }

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
    glm::vec3 ComputeSmoothNormalFrom4(const glm::vec3& v01,
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