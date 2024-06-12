#pragma once

namespace Isonia::Noise
{
    // Noise
    struct VirtualNoise
    {
    public:
        VirtualNoise(const unsigned int seed);

        virtual inline float generateNoise(const float x, const float y) const = 0;
        virtual inline float generateNoise(const float x, const float y, const float z) const = 0;
        virtual inline float generateNoise(const float x, const float y, const float z, const float w) const = 0;

    protected:
        const unsigned int seed;
    };

    struct WhiteNoise : public VirtualNoise
    {
    public:
        WhiteNoise(const unsigned int seed = 0);

    protected:
        const float x_constant;
        const float y_constant;
        const float z_constant;
        const float w_constant;
    };

    static constexpr const unsigned int prime_x = 501125321;
    static constexpr const unsigned int prime_y = 1136930381;
    static constexpr const unsigned int prime_z = 1338452629;
    static constexpr const unsigned int prime_w = 1720413743;

    struct PerlinNoise : public VirtualNoise
    {
    public:
        PerlinNoise(const unsigned int seed = 0);
    };

    struct FractalPerlinNoise : public PerlinNoise
    {
    public:
        FractalPerlinNoise(const unsigned int seed, const unsigned int octaves, const float lacunarity, const float gain, const float weighted_strength);

    protected:
        const unsigned int octaves;
        const float lacunarity;
        const float gain;
        const float weighted_strength;

        const float fractal_bounding;
        const float calculateFractalBounding() const;
    };

    // Warped Noise
    struct VirtualWarpNoise
    {
    public:
        VirtualWarpNoise();

        virtual inline void transformCoordinate(float* x, float* y) const = 0;
        virtual inline void transformCoordinate(float* x, float* y, float* z) const = 0;
        virtual inline void transformCoordinate(float* x, float* y, float* z, float* w) const = 0;
    };

    struct ConstantScalarWarpNoise : public VirtualWarpNoise
    {
    public:
        ConstantScalarWarpNoise(const float frequency);

    protected:
        const float frequency;
    };
}
