namespace Isonia::Noise
{
	FractalPerlinNoise::FractalPerlinNoise(
		const unsigned int seed, const unsigned int octaves, const float lacunarity, const float gain, const float weighted_strength) :
		PerlinNoise(seed),
		octaves(octaves), lacunarity(lacunarity), gain(gain), weighted_strength(weighted_strength), fractal_bounding(calculateFractalBounding()
	) { }

	inline float FractalPerlinNoise::generateNoise(const float x, const float y) const
	{
		unsigned int seed = this->seed;
		float sum = 0.0f;
		float amp = fractal_bounding;

		for (unsigned int i = 0; i < octaves; i++)
		{
			float noise = PerlinNoise::generateNoise(seed++, x, y);
			sum += noise * amp;
			amp *= Math::lerpf(1.0f, Math::Min(noise + 1.0f, 2.0f) * 0.5f, weighted_strength);

			x *= lacunarity;
			y *= lacunarity;
			amp *= gain;
		}

		return sum;
	}
	inline float FractalPerlinNoise::generateNoise(const float x, const float y, const float z) const
	{
		unsigned int seed = this->seed;
		float sum = 0.0f;
		float amp = fractal_bounding;

		for (unsigned int i = 0; i < octaves; i++)
		{
			float noise = PerlinNoise::generateNoise(seed++, x, y, z);
			sum += noise * amp;
			amp *= Math::lerpf(1.0f, (noise + 1.0f) * 0.5f, weighted_strength);

			x *= lacunarity;
			y *= lacunarity;
			z *= lacunarity;
			amp *= gain;
		}

		return sum;
	}
	inline float FractalPerlinNoise::generateNoise(const float x, const float y, const float z, const float w) const
	{
		unsigned int seed = this->seed;
		float sum = 0.0f;
		float amp = fractal_bounding;

		for (unsigned int i = 0; i < octaves; i++)
		{
			float noise = PerlinNoise::generateNoise(seed++, x, y, z, w);
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

	const inline float FractalPerlinNoise::calculateFractalBounding() const
	{
		float gain = Math::absf(this->gain);
		float amp = gain;
		float ampFractal = 1.0f;
		for (unsigned int i = 1; i < octaves; i++)
		{
			ampFractal += amp;
			amp *= gain;
		}
		return 1.0f / ampFractal;
	}
}
