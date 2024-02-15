#pragma once

// internal
#include "../Utilities/MathUtility.h"
#include "../Utilities/HashUtility.h"

namespace IMath = Isonia::Utilities::Math;

namespace Isonia::Noise
{
    class WarpNoise
    {
    public:
        WarpNoise()
        {
        }

        virtual inline void TransformCoordinate(float& x, float& y) const = 0;
        virtual inline void TransformCoordinate(float& x, float& y, float& z) const = 0;
        virtual inline void TransformCoordinate(float& x, float& y, float& z, float& t) const = 0;
    };
}
