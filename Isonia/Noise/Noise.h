#pragma once

// internal
#include "../Math/Hash.h"

namespace Isonia::Noise
{
    class Noise
    {
    public:
        Noise(const int32_t seed) : seed(seed)
        {
        }

        virtual inline float GenerateNoise(float x, float y) const = 0;
        virtual inline float GenerateNoise(float x, float y, float z) const = 0;
        virtual inline float GenerateNoise(float x, float y, float z, float t) const = 0;

    protected:
        const int32_t seed;
    };
}
