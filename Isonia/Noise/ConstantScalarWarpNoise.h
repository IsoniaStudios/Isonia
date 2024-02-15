#pragma once

// internal
#include "WarpNoise.h"

namespace Isonia::Noise
{
    class ConstantScalarWarpNoise : public WarpNoise
    {
    public:
        ConstantScalarWarpNoise(const float frequency) : WarpNoise(), frequency(frequency)
        {
        }

        inline void TransformCoordinate(float& x, float& y) const
        {
            x *= frequency;
            y *= frequency;
        }

        inline void TransformCoordinate(float& x, float& y, float& z) const
        {
            x *= frequency;
            y *= frequency;
            z *= frequency;
        }

        inline void TransformCoordinate(float& x, float& y, float& z, float& t) const
        {
            x *= frequency;
            y *= frequency;
            z *= frequency;
            t *= frequency;
        }

    protected:
        const float frequency;
    };
}
