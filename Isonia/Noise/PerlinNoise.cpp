// internal
#include "Noise.h"
#include "../Math/Math.h"

namespace Isonia::Noise
{
	PerlinNoise::PerlinNoise(const unsigned int seed) : VirtualNoise(seed) { }

	inline float PerlinNoise::generateNoise(const float x, const float y) const
	{
		return generatePerlinNoise(seed, x, y);
	}
	inline float PerlinNoise::generateNoise(const float x, const float y, const float z) const
	{
		return generatePerlinNoise(seed, x, y, z);
	}
	inline float PerlinNoise::generateNoise(const float x, const float y, const float z, const float w) const
	{
		return generatePerlinNoise(seed, x, y, z, w);
	}

	inline float PerlinNoise::generatePerlinNoise(const int seed, const float x, const float y) const
	{
		unsigned int x0 = Math::floorf_i(x);
		unsigned int y0 = Math::floorf_i(y);

		float xd0 = (float)(x - x0);
		float yd0 = (float)(y - y0);
		float xd1 = xd0 - 1;
		float yd1 = yd0 - 1;

		float xs = Math::interpolationQuinticf(xd0);
		float ys = Math::interpolationQuinticf(yd0);

		x0 *= prime_x;
		y0 *= prime_y;
		unsigned int x1 = x0 + prime_x;
		unsigned int y1 = y0 + prime_y;

		float xf0 = Math::lerpf(Math::gradCoord(seed, x0, y0, xd0, yd0),
								Math::gradCoord(seed, x1, y0, xd1, yd0), xs);
		float xf1 = Math::lerpf(Math::gradCoord(seed, x0, y1, xd0, yd1),
								Math::gradCoord(seed, x1, y1, xd1, yd1), xs);

		return Math::lerpf(xf0, xf1, ys) * 1.4247691104677813f;
	}
	inline float PerlinNoise::generatePerlinNoise(const int seed, const float x, const float y, const float z) const
	{
		unsigned int x0 = Math::floorf_i(x);
		unsigned int y0 = Math::floorf_i(y);
		unsigned int z0 = Math::floorf_i(z);

		float xd0 = (float)(x - x0);
		float yd0 = (float)(y - y0);
		float zd0 = (float)(z - z0);
		float xd1 = xd0 - 1;
		float yd1 = yd0 - 1;
		float zd1 = zd0 - 1;

		float xs = Math::interpolationQuinticf(xd0);
		float ys = Math::interpolationQuinticf(yd0);
		float zs = Math::interpolationQuinticf(zd0);

		x0 *= prime_x;
		y0 *= prime_y;
		z0 *= prime_z;
		unsigned int x1 = x0 + prime_x;
		unsigned int y1 = y0 + prime_y;
		unsigned int z1 = z0 + prime_z;

		float xf00 = Math::lerpf(Math::gradCoord(seed, x0, y0, z0, xd0, yd0, zd0),
								 Math::gradCoord(seed, x1, y0, z0, xd1, yd0, zd0), xs);
		float xf10 = Math::lerpf(Math::gradCoord(seed, x0, y1, z0, xd0, yd1, zd0),
								 Math::gradCoord(seed, x1, y1, z0, xd1, yd1, zd0), xs);
		float xf01 = Math::lerpf(Math::gradCoord(seed, x0, y0, z1, xd0, yd0, zd1),
								 Math::gradCoord(seed, x1, y0, z1, xd1, yd0, zd1), xs);
		float xf11 = Math::lerpf(Math::gradCoord(seed, x0, y1, z1, xd0, yd1, zd1),
								 Math::gradCoord(seed, x1, y1, z1, xd1, yd1, zd1), xs);

		float yf0 = Math::lerpf(xf00, xf10, ys);
		float yf1 = Math::lerpf(xf01, xf11, ys);

		return Math::lerpf(yf0, yf1, zs) * 0.964921414852142333984375f;
	}
	inline float PerlinNoise::generatePerlinNoise(const int seed, const float x, const float y, const float z, const float w) const
	{
		unsigned int x0 = Math::floorf_i(x);
		unsigned int y0 = Math::floorf_i(y);
		unsigned int z0 = Math::floorf_i(z);
		unsigned int w0 = Math::floorf_i(w);

		float xd0 = (float)(x - x0);
		float yd0 = (float)(y - y0);
		float zd0 = (float)(z - z0);
		float wd0 = (float)(w - w0);
		float xd1 = xd0 - 1;
		float yd1 = yd0 - 1;
		float zd1 = zd0 - 1;
		float wd1 = wd0 - 1;

		float xs = Math::interpolationQuinticf(xd0);
		float ys = Math::interpolationQuinticf(yd0);
		float zs = Math::interpolationQuinticf(zd0);
		float ws = Math::interpolationQuinticf(wd0);

		x0 *= prime_x;
		y0 *= prime_y;
		z0 *= prime_z;
		w0 *= prime_w;
		unsigned int x1 = x0 + prime_x;
		unsigned int y1 = y0 + prime_y;
		unsigned int z1 = z0 + prime_z;
		unsigned int w1 = w0 + prime_w;

		float xf000 = Math::lerpf(Math::gradCoord(seed, x0, y0, z0, w0, xd0, yd0, zd0, wd0),
								  Math::gradCoord(seed, x1, y0, z0, w0, xd1, yd0, zd0, wd0), xs);
		float xf100 = Math::lerpf(Math::gradCoord(seed, x0, y1, z0, w0, xd0, yd1, zd0, wd0),
								  Math::gradCoord(seed, x1, y1, z0, w0, xd1, yd1, zd0, wd0), xs);
		float xf010 = Math::lerpf(Math::gradCoord(seed, x0, y0, z1, w0, xd0, yd0, zd1, wd0),
								  Math::gradCoord(seed, x1, y0, z1, w0, xd1, yd0, zd1, wd0), xs);
		float xf110 = Math::lerpf(Math::gradCoord(seed, x0, y1, z1, w0, xd0, yd1, zd1, wd0),
								  Math::gradCoord(seed, x1, y1, z1, w0, xd1, yd1, zd1, wd0), xs);
		float xf001 = Math::lerpf(Math::gradCoord(seed, x0, y0, z0, w1, xd0, yd0, zd0, wd1),
								  Math::gradCoord(seed, x1, y0, z0, w1, xd1, yd0, zd0, wd1), xs);
		float xf101 = Math::lerpf(Math::gradCoord(seed, x0, y1, z0, w1, xd0, yd1, zd0, wd1),
								  Math::gradCoord(seed, x1, y1, z0, w1, xd1, yd1, zd0, wd1), xs);
		float xf011 = Math::lerpf(Math::gradCoord(seed, x0, y0, z1, w1, xd0, yd0, zd1, wd1),
								  Math::gradCoord(seed, x1, y0, z1, w1, xd1, yd0, zd1, wd1), xs);
		float xf111 = Math::lerpf(Math::gradCoord(seed, x0, y1, z1, w1, xd0, yd1, zd1, wd1),
								  Math::gradCoord(seed, x1, y1, z1, w1, xd1, yd1, zd1, wd1), xs);

		float yf00 = Math::lerpf(xf000, xf100, ys);
		float yf10 = Math::lerpf(xf010, xf110, ys);
		float yf01 = Math::lerpf(xf001, xf101, ys);
		float yf11 = Math::lerpf(xf011, xf111, ys);

		float zf0 = Math::lerpf(yf00, yf10, zs);
		float zf1 = Math::lerpf(yf01, yf11, zs);

		return Math::lerpf(zf0, zf1, ws) * 0.964921414852142333984375f;
	}
}
