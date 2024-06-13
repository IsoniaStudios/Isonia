// internal
#include "Math.h"

namespace Isonia::Math
{
    extern inline constexpr float interpolationHermitef(const float t)
    {
        return t * t * (3.0f - 2.0f * t);
    }
    extern inline constexpr float interpolationQuinticf(const float t)
    {
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    extern inline constexpr float lerpf(const float a, const float b, const float t)
    {
        return a + t * (b - a);
    }
    extern inline constexpr Vector3 lerpv3(const Vector3* a, const Vector3* b, const float t)
    {
        return Vector3{
            lerpf(a->x, b->x, t),
            lerpf(a->y, b->y, t),
            lerpf(a->z, b->z, t)
        };
    }
    extern inline constexpr float cubicLerpf(const float a, const float b, const float c, const float d, const float t)
    {
        const float p = (d - c) - (a - b);
        return t * t * t * p + t * t * ((a - b) - p) + t * (c - a) + b;
    }
    extern inline constexpr Vector3 cubicLerpv3(const Vector3* a, const Vector3* b, const Vector3* c, const Vector3* d, const float t)
    {
        return Vector3{
            cubicLerpf(a->x, b->x, c->x, d->x, t),
            cubicLerpf(a->y, b->y, c->y, d->y, t),
            cubicLerpf(a->z, b->z, c->z, d->z, t)
        };
    }

    extern inline constexpr Vector3 smoothNormalFromFour(const float* height_map, const float dx, const float dz, const unsigned int width, const unsigned int height, const unsigned int z, const unsigned int x)
    {
        const unsigned int middle_index = z * width + x;
        const float top = height_map[middle_index - width];
        const float left = height_map[middle_index - 1];
        const float right = height_map[middle_index + 1];
        const float bottom = height_map[middle_index + width];

        return vec3Normalize(
            bottom - top,
            -dx - dz,
            right - left
        );
    }
    extern inline constexpr Vector3 smoothNormalFromEight(const float* height_map, const float dx, const float dz, const unsigned int width, const unsigned int height, const unsigned int z, const unsigned int x)
    {
        const unsigned int middle_index = z * width + x;
        const float top = height_map[middle_index - width];
        const float top_left = height_map[middle_index - width - 1];
        const float top_right = height_map[middle_index - width + 1];
        const float left = height_map[middle_index - 1];
        const float right = height_map[middle_index + 1];
        const float bottom = height_map[middle_index + width];
        const float bottom_left = height_map[middle_index + width - 1];
        const float bottom_right = height_map[middle_index + width + 1];

        return vec3Normalize(
            top_right + right + bottom_right - top_left - left - bottom_left,
            -dx - dz - dx - dz,
            bottom_left + bottom + bottom_right - top_left - top - top_right
        );
    }
}
