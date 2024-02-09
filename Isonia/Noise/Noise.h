#pragma once

// internal
#include "../Utilities/MathUtility.h"
#include "../Utilities/HashUtility.h"

namespace IMath = Isonia::Utilities::Math;

namespace Isonia::Noise
{
    static constexpr const inline float Gradients2D[] =
    {
        0.130526192220052f, 0.99144486137381f, 0.38268343236509f, 0.923879532511287f, 0.608761429008721f, 0.793353340291235f, 0.793353340291235f, 0.608761429008721f,
        0.923879532511287f, 0.38268343236509f, 0.99144486137381f, 0.130526192220051f, 0.99144486137381f, -0.130526192220051f, 0.923879532511287f, -0.38268343236509f,
        0.793353340291235f, -0.60876142900872f, 0.608761429008721f, -0.793353340291235f, 0.38268343236509f, -0.923879532511287f, 0.130526192220052f, -0.99144486137381f,
        -0.130526192220052f, -0.99144486137381f, -0.38268343236509f, -0.923879532511287f, -0.608761429008721f, -0.793353340291235f, -0.793353340291235f, -0.608761429008721f,
        -0.923879532511287f, -0.38268343236509f, -0.99144486137381f, -0.130526192220052f, -0.99144486137381f, 0.130526192220051f, -0.923879532511287f, 0.38268343236509f,
        -0.793353340291235f, 0.608761429008721f, -0.608761429008721f, 0.793353340291235f, -0.38268343236509f, 0.923879532511287f, -0.130526192220052f, 0.99144486137381f,
        0.130526192220052f, 0.99144486137381f, 0.38268343236509f, 0.923879532511287f, 0.608761429008721f, 0.793353340291235f, 0.793353340291235f, 0.608761429008721f,
        0.923879532511287f, 0.38268343236509f, 0.99144486137381f, 0.130526192220051f, 0.99144486137381f, -0.130526192220051f, 0.923879532511287f, -0.38268343236509f,
        0.793353340291235f, -0.60876142900872f, 0.608761429008721f, -0.793353340291235f, 0.38268343236509f, -0.923879532511287f, 0.130526192220052f, -0.99144486137381f,
        -0.130526192220052f, -0.99144486137381f, -0.38268343236509f, -0.923879532511287f, -0.608761429008721f, -0.793353340291235f, -0.793353340291235f, -0.608761429008721f,
        -0.923879532511287f, -0.38268343236509f, -0.99144486137381f, -0.130526192220052f, -0.99144486137381f, 0.130526192220051f, -0.923879532511287f, 0.38268343236509f,
        -0.793353340291235f, 0.608761429008721f, -0.608761429008721f, 0.793353340291235f, -0.38268343236509f, 0.923879532511287f, -0.130526192220052f, 0.99144486137381f,
        0.130526192220052f, 0.99144486137381f, 0.38268343236509f, 0.923879532511287f, 0.608761429008721f, 0.793353340291235f, 0.793353340291235f, 0.608761429008721f,
        0.923879532511287f, 0.38268343236509f, 0.99144486137381f, 0.130526192220051f, 0.99144486137381f, -0.130526192220051f, 0.923879532511287f, -0.38268343236509f,
        0.793353340291235f, -0.60876142900872f, 0.608761429008721f, -0.793353340291235f, 0.38268343236509f, -0.923879532511287f, 0.130526192220052f, -0.99144486137381f,
        -0.130526192220052f, -0.99144486137381f, -0.38268343236509f, -0.923879532511287f, -0.608761429008721f, -0.793353340291235f, -0.793353340291235f, -0.608761429008721f,
        -0.923879532511287f, -0.38268343236509f, -0.99144486137381f, -0.130526192220052f, -0.99144486137381f, 0.130526192220051f, -0.923879532511287f, 0.38268343236509f,
        -0.793353340291235f, 0.608761429008721f, -0.608761429008721f, 0.793353340291235f, -0.38268343236509f, 0.923879532511287f, -0.130526192220052f, 0.99144486137381f,
        0.130526192220052f, 0.99144486137381f, 0.38268343236509f, 0.923879532511287f, 0.608761429008721f, 0.793353340291235f, 0.793353340291235f, 0.608761429008721f,
        0.923879532511287f, 0.38268343236509f, 0.99144486137381f, 0.130526192220051f, 0.99144486137381f, -0.130526192220051f, 0.923879532511287f, -0.38268343236509f,
        0.793353340291235f, -0.60876142900872f, 0.608761429008721f, -0.793353340291235f, 0.38268343236509f, -0.923879532511287f, 0.130526192220052f, -0.99144486137381f,
        -0.130526192220052f, -0.99144486137381f, -0.38268343236509f, -0.923879532511287f, -0.608761429008721f, -0.793353340291235f, -0.793353340291235f, -0.608761429008721f,
        -0.923879532511287f, -0.38268343236509f, -0.99144486137381f, -0.130526192220052f, -0.99144486137381f, 0.130526192220051f, -0.923879532511287f, 0.38268343236509f,
        -0.793353340291235f, 0.608761429008721f, -0.608761429008721f, 0.793353340291235f, -0.38268343236509f, 0.923879532511287f, -0.130526192220052f, 0.99144486137381f,
        0.130526192220052f, 0.99144486137381f, 0.38268343236509f, 0.923879532511287f, 0.608761429008721f, 0.793353340291235f, 0.793353340291235f, 0.608761429008721f,
        0.923879532511287f, 0.38268343236509f, 0.99144486137381f, 0.130526192220051f, 0.99144486137381f, -0.130526192220051f, 0.923879532511287f, -0.38268343236509f,
        0.793353340291235f, -0.60876142900872f, 0.608761429008721f, -0.793353340291235f, 0.38268343236509f, -0.923879532511287f, 0.130526192220052f, -0.99144486137381f,
        -0.130526192220052f, -0.99144486137381f, -0.38268343236509f, -0.923879532511287f, -0.608761429008721f, -0.793353340291235f, -0.793353340291235f, -0.608761429008721f,
        -0.923879532511287f, -0.38268343236509f, -0.99144486137381f, -0.130526192220052f, -0.99144486137381f, 0.130526192220051f, -0.923879532511287f, 0.38268343236509f,
        -0.793353340291235f, 0.608761429008721f, -0.608761429008721f, 0.793353340291235f, -0.38268343236509f, 0.923879532511287f, -0.130526192220052f, 0.99144486137381f,
        0.38268343236509f, 0.923879532511287f, 0.923879532511287f, 0.38268343236509f, 0.923879532511287f, -0.38268343236509f, 0.38268343236509f, -0.923879532511287f,
        -0.38268343236509f, -0.923879532511287f, -0.923879532511287f, -0.38268343236509f, -0.923879532511287f, 0.38268343236509f, -0.38268343236509f, 0.923879532511287f,
    };

    static constexpr const inline float Gradients3D[] =
    {
        0, 1, 1, 0,  0,-1, 1, 0,  0, 1,-1, 0,  0,-1,-1, 0,
        1, 0, 1, 0, -1, 0, 1, 0,  1, 0,-1, 0, -1, 0,-1, 0,
        1, 1, 0, 0, -1, 1, 0, 0,  1,-1, 0, 0, -1,-1, 0, 0,
        0, 1, 1, 0,  0,-1, 1, 0,  0, 1,-1, 0,  0,-1,-1, 0,
        1, 0, 1, 0, -1, 0, 1, 0,  1, 0,-1, 0, -1, 0,-1, 0,
        1, 1, 0, 0, -1, 1, 0, 0,  1,-1, 0, 0, -1,-1, 0, 0,
        0, 1, 1, 0,  0,-1, 1, 0,  0, 1,-1, 0,  0,-1,-1, 0,
        1, 0, 1, 0, -1, 0, 1, 0,  1, 0,-1, 0, -1, 0,-1, 0,
        1, 1, 0, 0, -1, 1, 0, 0,  1,-1, 0, 0, -1,-1, 0, 0,
        0, 1, 1, 0,  0,-1, 1, 0,  0, 1,-1, 0,  0,-1,-1, 0,
        1, 0, 1, 0, -1, 0, 1, 0,  1, 0,-1, 0, -1, 0,-1, 0,
        1, 1, 0, 0, -1, 1, 0, 0,  1,-1, 0, 0, -1,-1, 0, 0,
        0, 1, 1, 0,  0,-1, 1, 0,  0, 1,-1, 0,  0,-1,-1, 0,
        1, 0, 1, 0, -1, 0, 1, 0,  1, 0,-1, 0, -1, 0,-1, 0,
        1, 1, 0, 0, -1, 1, 0, 0,  1,-1, 0, 0, -1,-1, 0, 0,
        1, 1, 0, 0,  0,-1, 1, 0, -1, 1, 0, 0,  0,-1,-1, 0
    };

    static constexpr const inline int PrimeX = 501125321;
    static constexpr const inline int PrimeY = 1136930381;
    static constexpr const inline int PrimeZ = 1720413743;

    static inline float Random(const float from, const float to)
    {
        return from + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (to - from)));
    }

	class Noise
	{
	public:
        Noise(const int seed, const float frequency) : seed(seed), frequency(frequency),
                                                       octaves(0), lacunarity(0), gain(0), weightedStrength(0), fractalBounding(0)
        {

        }
        Noise(int seed, float frequency,
              const int octaves, const float lacunarity, const float gain, const float weightedStrength) :
              seed(seed), frequency(frequency),
              octaves(octaves), lacunarity(lacunarity), gain(gain), weightedStrength(weightedStrength), fractalBounding(CalculateFractalBounding())
        {
            
        }

        inline float GenerateNoise(float x, float y) const
        {
            TransformCoordinate(x, y);

            return GeneratePerlinNoise(seed, x, y);
        }

        inline float GenerateNoise(float x, float y, float z) const
        {
            TransformCoordinate(x, y, z);

            return GeneratePerlinNoise(seed, x, y, z);
        }

        inline float GenerateFractalNoise(float x, float y) const
        {
            TransformCoordinate(x, y);

            return GenerateFractalPerlinNoise(seed, x, y);
        }

        inline float GenerateFractalNoise(float x, float y, float z) const
        {
            TransformCoordinate(x, y, z);

            return GeneratePerlinNoise(seed, x, y, z);
        }

        inline float GenerateNoiseAndModifyCoordinate(float& x, float& y) const
        {
            TransformCoordinate(x, y);

            return GeneratePerlinNoise(seed, x, y);
        }

        inline float GenerateNoiseAndModifyCoordinate(float& x, float& y, float& z) const
        {
            TransformCoordinate(x, y, z);

            return GeneratePerlinNoise(seed, x, y, z);
        }

        inline float GenerateFractalNoiseAndModifyCoordinate(float& x, float& y) const
        {
            TransformCoordinate(x, y);

            return GeneratePerlinNoise(seed, x, y);
        }

        inline float GenerateFractalNoiseAndModifyCoordinate(float& x, float& y, float& z) const
        {
            TransformCoordinate(x, y, z);

            return GeneratePerlinNoise(seed, x, y, z);
        }

	private:
		const int seed;
        const float frequency;

        const int octaves;
        const float lacunarity;
        const float gain;
        const float weightedStrength;

        const float fractalBounding;
        const inline float CalculateFractalBounding() const
        {
            float gain = IMath::Abs(this->gain);
            float amp = gain;
            float ampFractal = 1.0f;
            for (int i = 1; i < octaves; i++)
            {
                ampFractal += amp;
                amp *= gain;
            }
            return 1.0f / ampFractal;
        }

        const inline void TransformCoordinate(float& x, float& y) const
        {
            x *= frequency;
            y *= frequency;
        }

        const inline void TransformCoordinate(float& x, float& y, float& z) const
        {
            x *= frequency;
            y *= frequency;
            z *= frequency;
        }

        static const inline float GeneratePerlinNoise(const int seed, const float x, const float y)
        {
            int x0 = IMath::FloorToInt(x);
            int y0 = IMath::FloorToInt(y);

            float xd0 = (float)(x - x0);
            float yd0 = (float)(y - y0);
            float xd1 = xd0 - 1;
            float yd1 = yd0 - 1;

            float xs = IMath::InterpolationQuintic(xd0);
            float ys = IMath::InterpolationQuintic(yd0);

            x0 *= PrimeX;
            y0 *= PrimeY;
            int x1 = x0 + PrimeX;
            int y1 = y0 + PrimeY;

            float xf0 = IMath::Lerp(GradCoord(seed, x0, y0, xd0, yd0), GradCoord(seed, x1, y0, xd1, yd0), xs);
            float xf1 = IMath::Lerp(GradCoord(seed, x0, y1, xd0, yd1), GradCoord(seed, x1, y1, xd1, yd1), xs);

            return IMath::Lerp(xf0, xf1, ys) * 1.4247691104677813f;
        }

        static const inline float GeneratePerlinNoise(const int seed, const float x, const float y, const float z)
        {
            int x0 = IMath::FloorToInt(x);
            int y0 = IMath::FloorToInt(y);
            int z0 = IMath::FloorToInt(z);

            float xd0 = (float)(x - x0);
            float yd0 = (float)(y - y0);
            float zd0 = (float)(z - z0);
            float xd1 = xd0 - 1;
            float yd1 = yd0 - 1;
            float zd1 = zd0 - 1;

            float xs = IMath::InterpolationQuintic(xd0);
            float ys = IMath::InterpolationQuintic(yd0);
            float zs = IMath::InterpolationQuintic(zd0);

            x0 *= PrimeX;
            y0 *= PrimeY;
            z0 *= PrimeZ;
            int x1 = x0 + PrimeX;
            int y1 = y0 + PrimeY;
            int z1 = z0 + PrimeZ;

            float xf00 = IMath::Lerp(GradCoord(seed, x0, y0, z0, xd0, yd0, zd0), GradCoord(seed, x1, y0, z0, xd1, yd0, zd0), xs);
            float xf10 = IMath::Lerp(GradCoord(seed, x0, y1, z0, xd0, yd1, zd0), GradCoord(seed, x1, y1, z0, xd1, yd1, zd0), xs);
            float xf01 = IMath::Lerp(GradCoord(seed, x0, y0, z1, xd0, yd0, zd1), GradCoord(seed, x1, y0, z1, xd1, yd0, zd1), xs);
            float xf11 = IMath::Lerp(GradCoord(seed, x0, y1, z1, xd0, yd1, zd1), GradCoord(seed, x1, y1, z1, xd1, yd1, zd1), xs);

            float yf0 = IMath::Lerp(xf00, xf10, ys);
            float yf1 = IMath::Lerp(xf01, xf11, ys);

            return IMath::Lerp(yf0, yf1, zs) * 0.964921414852142333984375f;
        }

        const inline float GenerateFractalPerlinNoise(float x, float y) const
        {
            int seed = this->seed;
            float sum = 0;
            float amp = fractalBounding;

            for (int i = 0; i < octaves; i++)
            {
                float noise = GeneratePerlinNoise(seed++, x, y);
                sum += noise * amp;
                amp *= IMath::Lerp(1.0f, IMath::Min(noise + 1, 2) * 0.5f, weightedStrength);

                x *= lacunarity;
                y *= lacunarity;
                amp *= gain;
            }

            return sum;
        }

        const inline float GenerateFractalPerlinNoise(float x, float y, float z) const
        {
            int seed = this->seed;
            float sum = 0;
            float amp = fractalBounding;

            for (int i = 0; i < octaves; i++)
            {
                float noise = GeneratePerlinNoise(seed++, x, y, z);
                sum += noise * amp;
                amp *= IMath::Lerp(1.0f, (noise + 1) * 0.5f, weightedStrength);

                x *= lacunarity;
                y *= lacunarity;
                z *= lacunarity;
                amp *= gain;
            }

            return sum;
        }

        static const inline int Hash(const int seed, const int xPrimed, const int yPrimed)
        {
            int hash = seed ^ xPrimed ^ yPrimed;

            hash *= 0x27d4eb2d;
            return hash;
        }

        static const inline int Hash(const int seed, const int xPrimed, const int yPrimed, const int zPrimed)
        {
            int hash = seed ^ xPrimed ^ yPrimed ^ zPrimed;

            hash *= 0x27d4eb2d;
            return hash;
        }

        static const inline float ValCoord(const int seed, const int xPrimed, const int yPrimed)
        {
            int hash = Hash(seed, xPrimed, yPrimed);

            hash *= hash;
            hash ^= hash << 19;
            return hash * (1 / 2147483648.0f);
        }

        static const inline float ValCoord(const int seed, const int xPrimed, const int yPrimed, const int zPrimed)
        {
            int hash = Hash(seed, xPrimed, yPrimed, zPrimed);

            hash *= hash;
            hash ^= hash << 19;
            return hash * (1 / 2147483648.0f);
        }

        static const inline float GradCoord(const int seed, const int xPrimed, const int yPrimed, const float xd, const float yd)
        {
            int hash = Hash(seed, xPrimed, yPrimed);
            hash ^= hash >> 15;
            hash &= 127 << 1;

            float xg = Gradients2D[hash];
            float yg = Gradients2D[hash | 1];

            return xd * xg + yd * yg;
        }

        static const inline float GradCoord(const int seed, const int xPrimed, const int yPrimed, const int zPrimed, const float xd, const float yd, const float zd)
        {
            int hash = Hash(seed, xPrimed, yPrimed, zPrimed);
            hash ^= hash >> 15;
            hash &= 63 << 2;

            float xg = Gradients3D[hash];
            float yg = Gradients3D[hash | 1];
            float zg = Gradients3D[hash | 2];

            return xd * xg + yd * yg + zd * zg;
        }
	};
}
