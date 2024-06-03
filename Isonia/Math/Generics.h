#pragma once

// std
#include <cmath>
#include <limits>

namespace Isonia::Math
{
    // Constants
    static constexpr const float Epsilon = std::numeric_limits<float>::epsilon();

    //
    static inline float Mod(const float x, const float y) { return std::fmodf(x, y); }

    static inline float Min(const float a, const float b) { return a < b ? a : b; }

    static inline float Max(const float a, const float b) { return a > b ? a : b; }

    static inline float Clamp(const float f, const float min, const float max) { return (f < min) ? min : (f > max) ? max : f; }

    static inline float Abs(const float f) { return f < 0 ? -f : f; }

    static inline float Sqrt(const float f) { return std::sqrtf(f); }

    // To-Int
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
}
