// internal
#include "Noise.h"

namespace Isonia::Noise
{
    WhiteNoise::WhiteNoise(const unsigned int seed) : VirtualNoise(seed),
        x_constant(static_cast<float>(seed) + 12.9898f),
        y_constant(static_cast<float>(seed) + 78.2332f),
        z_constant(static_cast<float>(seed) + 53.5395f),
        w_constant(static_cast<float>(seed) + 94.4353f)
    { }

    inline float WhiteNoise::generateNoise(const float x, const float y) const
    {
        return Math::sinf(x * x_constant + y * y_constant);
    }
    inline float WhiteNoise::generateNoise(const float x, const float y, const float z) const
    {
        return Math::sinf(x * x_constant + y * y_constant + z * z_constant);
    }
    inline float WhiteNoise::generateNoise(const float x, const float y, float z, const float w) const
    {
        return Math::sinf(x * x_constant + y * y_constant + z * z_constant + w * w_constant);
    }
}
