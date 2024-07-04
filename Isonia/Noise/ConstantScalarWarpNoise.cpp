// internal
#include "Noise.h"

namespace Isonia::Noise
{
    ConstantScalarWarpNoise::ConstantScalarWarpNoise(const float frequency)
        : VirtualWarpNoise(), frequency(frequency)
    {
    }

    inline void ConstantScalarWarpNoise::transformCoordinate(float* x, float* y) const
    {
        *x *= frequency;
        *y *= frequency;
    }
    inline void ConstantScalarWarpNoise::transformCoordinate(float* x, float* y, float* z) const
    {
        *x *= frequency;
        *y *= frequency;
        *z *= frequency;
    }
    inline void ConstantScalarWarpNoise::transformCoordinate(float* x, float* y, float* z, float* w) const
    {
        *x *= frequency;
        *y *= frequency;
        *z *= frequency;
        *w *= frequency;
    }
}
