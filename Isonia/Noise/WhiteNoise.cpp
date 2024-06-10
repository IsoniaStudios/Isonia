namespace Isonia::Noise
{
    WhiteNoise::WhiteNoise(const unsigned int seed) : VirtualNoise(seed),
        xConstant(static_cast<float>(seed) + 12.9898f),
        yConstant(static_cast<float>(seed) + 78.2332f),
        zConstant(static_cast<float>(seed) + 53.5395f),
        wConstant(static_cast<float>(seed) + 94.4353f)
    {
    }

    inline float WhiteNoise::generateNoise(const float x, const float y) const
    {
        return Math::sinf(x * xConstant + y * yConstant);
    }
    inline float WhiteNoise::generateNoise(const float x, const float y, const float z) const
    {
        return Math::sinf(x * xConstant + y * yConstant + z * zConstant);
    }
    inline float WhiteNoise::generateNoise(const float x, const float y, float z, const float w) const
    {
        return Math::sinf(x * xConstant + y * yConstant + z * zConstant + w * wConstant);
    }
}
