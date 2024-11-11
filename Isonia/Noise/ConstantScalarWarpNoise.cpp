// internal
#include "Noise.h"

namespace Isonia::Noise
{
    ConstantScalarWarpNoise::ConstantScalarWarpNoise(const float frequency)
        : VirtualWarpNoise(), frequency(frequency)
    {
    }

    void ConstantScalarWarpNoise::transformCoordinate(float* x, float* y) const
    {
        *x *= frequency;
        *y *= frequency;
    }
    void ConstantScalarWarpNoise::transformCoordinate(float* x, float* y, float* z) const
    {
        *x *= frequency;
        *y *= frequency;
        *z *= frequency;
    }
    void ConstantScalarWarpNoise::transformCoordinate(float* x, float* y, float* z, float* w) const
    {
        *x *= frequency;
        *y *= frequency;
        *z *= frequency;
        *w *= frequency;
    }
}
