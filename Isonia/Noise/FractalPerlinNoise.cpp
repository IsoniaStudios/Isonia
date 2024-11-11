// internal
#include "Noise.h"

namespace Isonia::Noise
{
	FractalPerlinNoise::FractalPerlinNoise(const unsigned int seed, const unsigned int octaves, const float lacunarity, const float gain, const float weighted_strength)
		: PerlinNoise(seed), octaves(octaves), lacunarity(lacunarity), gain(gain), weighted_strength(weighted_strength), fractal_bounding(calculateFractalBounding())
	{
	}

	float FractalPerlinNoise::generateNoise(float x, float y) const
	{
		unsigned int seed = this->seed;
		float sum = 0.0f;
		float amp = fractal_bounding;

		for (unsigned int i = 0; i < octaves; i++)
		{
			float noise = generatePerlinNoise(seed++, x, y);
			sum += noise * amp;
			amp *= Math::lerpf(1.0f, Math::minf(noise + 1.0f, 2.0f) * 0.5f, weighted_strength);

			x *= lacunarity;
			y *= lacunarity;
			amp *= gain;
		}

		return sum;
	}
	float FractalPerlinNoise::generateNoise(float x, float y, float z) const
	{
		unsigned int seed = this->seed;
		float sum = 0.0f;
		float amp = fractal_bounding;

		for (unsigned int i = 0; i < octaves; i++)
		{
			float noise = generatePerlinNoise(seed++, x, y, z);
			sum += noise * amp;
			amp *= Math::lerpf(1.0f, (noise + 1.0f) * 0.5f, weighted_strength);

			x *= lacunarity;
			y *= lacunarity;
			z *= lacunarity;
			amp *= gain;
		}

		return sum;
	}
	float FractalPerlinNoise::generateNoise(float x, float y, float z, float w) const
	{
		unsigned int seed = this->seed;
		float sum = 0.0f;
		float amp = fractal_bounding;

		for (unsigned int i = 0; i < octaves; i++)
		{
			float noise = generatePerlinNoise(seed++, x, y, z, w);
			sum += noise * amp;
			amp *= Math::lerpf(1.0f, (noise + 1.0f) * 0.5f, weighted_strength);

			x *= lacunarity;
			y *= lacunarity;
			z *= lacunarity;
			w *= lacunarity;
			amp *= gain;
		}

		return sum;
	}

	const float FractalPerlinNoise::calculateFractalBounding() const
	{
		float gain = Math::absf(this->gain);
		float amp = gain;
		float amp_fractal = 1.0f;
		for (unsigned int i = 1; i < octaves; i++)
		{
			amp_fractal += amp;
			amp *= gain;
		}
		return 1.0f / amp_fractal;
	}
}
