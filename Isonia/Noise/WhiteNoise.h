#pragma once

// internal
#include "Noise.h"

namespace Isonia::Noise
{
    class WhiteNoise : public Noise
    {
    public:
        WhiteNoise(const int32_t seed = 1) : Noise(seed),
            xConstant(static_cast<float>(seed) + 12.9898f),
            yConstant(static_cast<float>(seed) + 78.2332f),
            zConstant(static_cast<float>(seed) + 53.5395f),
            tConstant(static_cast<float>(seed) + 94.4353f)
        {
        }

        inline float GenerateNoise(float x, float y) const
        {
            return GenerateWhiteNoise(x, y);
        }

        inline float GenerateNoise(float x, float y, float z) const
        {
            return GenerateWhiteNoise(x, y, z);
        }

        inline float GenerateNoise(float x, float y, float z, float t) const
        {
            return GenerateWhiteNoise(x, y, z, t);
        }

    protected:
        const float xConstant;
        const float yConstant;
        const float zConstant;
        const float tConstant;

        inline float GenerateWhiteNoise(const float x, const float y) const
        {
            return sin(x * xConstant + y * yConstant);
        }
        inline float GenerateWhiteNoise(const float x, const float y, const float z) const
        {
            return sin(x * xConstant + y * yConstant + z * zConstant);
        }
        inline float GenerateWhiteNoise(const float x, const float y, const float z, const float t) const
        {
            return sin(x * xConstant + y * yConstant + z * zConstant + t * tConstant);
        }
    };
}
