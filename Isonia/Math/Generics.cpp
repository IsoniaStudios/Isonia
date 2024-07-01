// internal
#include "Math.h"

// external
#include <cmath>

namespace Isonia::Math
{
    extern inline constexpr float floorf(const float f)
    {
        const int truncated = static_cast<int>(f);
        if (f < truncated)
        {
            return static_cast<float>(truncated - 1);
        }
        return static_cast<float>(truncated);
    }

    extern inline constexpr float ceilf(const float f)
    {
        const int truncated = static_cast<int>(f);
        if (f > truncated)
        {
            return static_cast<float>(truncated + 1);
        }
        return static_cast<float>(truncated);
    }

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

    extern inline constexpr float truncf(const float f)
    {
        if (f > 0)
        {
            return floorf(f);
        }
        return ceilf(f);
    }

    extern inline constexpr int absi(const int i)
    {
        if (i < 0)
        {
            return -i;
        }
        return i;
    }

    extern inline constexpr float absf(const float f)
    {
        if (f < 0)
        {
            return -f;
        }
        return f;
    }

    extern inline float fmodf(const float x, const float y)
    {
        return x - truncf(x / y) * y;
    }

    extern inline constexpr int mini(const int a, const int b)
    {
        if (a < b)
        {
            return a;
        }
        return b;
    }
    extern inline constexpr int maxi(const int a, const int b)
    {
        if (a > b)
        {
            return a;
        }
        return b;
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
    extern inline constexpr unsigned int clampui(const unsigned int i, const unsigned int min, const unsigned int max)
    {
        if (i < min)
        {
            return min;
        }
        if (i > max)
        {
            return max;
        }
        return i;
    }
    extern inline constexpr int clampi(const int i, const int min, const int max)
    {
        if (i < min)
        {
            return min;
        }
        if (i > max)
        {
            return max;
        }
        return i;
    }

    extern inline constexpr float sqrtf(const float f)
    {
        return 1.0f / rsqrtf(f);
    }
    extern inline constexpr float rsqrtf(const float f)
    {
        long i = *(long*)&f;
        i = 0x5f3759df - (i >> 1);
        const float y = *(float*)&i;
        return y * (1.5f - (f * 0.5f * y * y));
    }
    extern inline constexpr float powf(const float f, const float e)
    {
        union {
            float f;
            unsigned int u;
        } temp = { f };
        temp.u = (unsigned int)(e * (float)(temp.u - 1064774667) + 1064774667);
        return temp.f;
    }
    extern inline constexpr float powPrecisef(const float f, const float e)
    {
        int e2 = (int)e;
        union {
            float f;
            unsigned int u;
        } temp = { f };
        temp.u = (unsigned int)((e - e2) * (float)(temp.u - 1064774667) + 1064774667);

        float r = 1.0f;
        float f2 = f;
        while (e2)
        {
            if (e2 & 1)
            {
                r *= f2;
            }
            f2 *= f2;
            e2 >>= 1;
        }

        return r * temp.f;
    }
    extern inline float powExactf(const float f, const float e)
    {
        return std::powf(f, e);
    }
}
