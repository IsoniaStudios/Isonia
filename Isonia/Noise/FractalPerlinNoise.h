#pragma once

// internal
#include "PerlinNoise.h"

namespace Isonia::Noise
{
	class FractalPerlinNoise : public PerlinNoise
	{
	public:
        FractalPerlinNoise(const int32_t seed,
                           const uint32_t octaves, const float lacunarity, const float gain, const float weightedStrength) :
                           PerlinNoise(seed),
                           octaves(octaves), lacunarity(lacunarity), gain(gain), weightedStrength(weightedStrength), fractalBounding(CalculateFractalBounding())
        {
        }

        inline float GenerateNoise(float x, float y) const
        {
            return GenerateFractalPerlinNoise(x, y);
        }

        inline float GenerateNoise(float x, float y, float z) const
        {
            return GenerateFractalPerlinNoise(x, y, z);
        }

        inline float GenerateNoise(float x, float y, float z, float t) const
        {
            return GenerateFractalPerlinNoise(x, y, z, t);
        }

	protected:
        const uint32_t octaves;
        const float lacunarity;
        const float gain;
        const float weightedStrength;

        const float fractalBounding;
        const inline float CalculateFractalBounding() const
        {
            float gain = Math::Abs(this->gain);
            float amp = gain;
            float ampFractal = 1.0f;
            for (uint32_t i = 1; i < octaves; i++)
            {
                ampFractal += amp;
                amp *= gain;
            }
            return 1.0f / ampFractal;
        }

        const inline float GenerateFractalPerlinNoise(float x, float y) const
        {
            int32_t seed = this->seed;
            float sum = 0.0f;
            float amp = fractalBounding;

            for (uint32_t i = 0; i < octaves; i++)
            {
                float noise = GeneratePerlinNoise(seed++, x, y);
                sum += noise * amp;
                amp *= Math::Lerp(1.0f, Math::Min(noise + 1.0f, 2.0f) * 0.5f, weightedStrength);

                x *= lacunarity;
                y *= lacunarity;
                amp *= gain;
            }

            return sum;
        }

        const inline float GenerateFractalPerlinNoise(float x, float y, float z) const
        {
            int32_t seed = this->seed;
            float sum = 0.0f;
            float amp = fractalBounding;

            for (uint32_t i = 0; i < octaves; i++)
            {
                float noise = GeneratePerlinNoise(seed++, x, y, z);
                sum += noise * amp;
                amp *= Math::Lerp(1.0f, (noise + 1.0f) * 0.5f, weightedStrength);

                x *= lacunarity;
                y *= lacunarity;
                z *= lacunarity;
                amp *= gain;
            }

            return sum;
        }

        const inline float GenerateFractalPerlinNoise(float x, float y, float z, float t) const
        {
            int32_t seed = this->seed;
            float sum = 0.0f;
            float amp = fractalBounding;

            for (uint32_t i = 0; i < octaves; i++)
            {
                float noise = GeneratePerlinNoise(seed++, x, y, z, t);
                sum += noise * amp;
                amp *= Math::Lerp(1.0f, (noise + 1.0f) * 0.5f, weightedStrength);

                x *= lacunarity;
                y *= lacunarity;
                z *= lacunarity;
                t *= lacunarity;
                amp *= gain;
            }

            return sum;
        }
	};
}
