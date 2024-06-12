// internal
#include "Math.h"

// std
#include <cmath>

namespace Isonia::Math
{
    extern inline constexpr float roundf(const float f)
    {
        const int integer_part = static_cast<int>(f);
        const float fractional_part = f - integer_part;

        if (fractional_part >= 0.5f)
        {
            return static_cast<float>(integer_part + 1);
        }
        if (fractional_part <= -0.5f)
        {
            return static_cast<float>(integer_part - 1);
        }
        return static_cast<float>(integer_part);
    }

    extern inline constexpr float absf(const float f)
    {
        if (f < 0)
        {
            return -f;
        }
        return f;
    }

    extern inline constexpr float fmodf(const float x, const float y)
    {
        return std::fmodf(x, y);
    }

    extern inline constexpr float minf(const float a, const float b)
    {
        if (a < b)
        {
            return a;
        }
        return b;
    }
    extern inline constexpr float maxf(const float a, const float b)
    {
        if (a > b)
        {
            return a;
        }
        return b;
    }
    extern inline constexpr float clampf(const float f, const float min, const float max)
    {
        if (f < min)
        {
            return min;
        }
        if (f > max)
        {
            return max;
        }
        return f;
    }

    extern inline constexpr float sqrtf(const float f)
    {
        return std::sqrtf(f);
    }
    extern inline constexpr float rsqrtf(const float f)
    {
        long i = *(long*)&f;
        i = 0x5f3759df - (i >> 1);
        const float y = *(float*)&i;
        return y * (1.5f - (f * 0.5f * y * y));
    }
}
