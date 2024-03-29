#pragma once

// internal
#include "Noise.h"
#include "ConstantScalarWarpNoise.h"

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
        0, 1, 1, 0,  0,-1, 1, 0, -1, 1, 0, 0,  0,-1,-1, 0,
    };

    static constexpr const inline float Gradients4D[] =
    {
        0, 1, 1, 0,  0,-1, 1, 0,  0, 1,-1, 0,  0,-1,-1, 0,
        1, 0, 1, 1, -1, 0, 1, 1,  1, 0,-1,-1, -1, 0,-1,-1,
        1, 1, 0, 1, -1, 1, 0, 1,  1,-1, 0,-1, -1,-1, 0,-1,
        0, 1, 1, 0,  0,-1, 1, 0,  0, 1,-1, 0,  0,-1,-1, 0,
        1, 0, 1, 1, -1, 0, 1, 1,  1, 0,-1,-1, -1, 0,-1,-1,
        1, 1, 0, 1, -1, 1, 0, 1,  1,-1, 0,-1, -1,-1, 0,-1,
        0, 1, 1, 0,  0,-1, 1, 0,  0, 1,-1, 0,  0,-1,-1, 0,
        1, 0, 1, 1, -1, 0, 1, 1,  1, 0,-1,-1, -1, 0,-1,-1,
        1, 1, 0, 1, -1, 1, 0, 1,  1,-1, 0,-1, -1,-1, 0,-1,
        0, 1, 1, 0,  0,-1, 1, 0,  0, 1,-1, 0,  0,-1,-1, 0,
        1, 0, 1, 1, -1, 0, 1, 1,  1, 0,-1,-1, -1, 0,-1,-1,
        1, 1, 0, 1, -1, 1, 0, 1,  1,-1, 0,-1, -1,-1, 0,-1,
        0, 1, 1, 0,  0,-1, 1, 0,  0, 1,-1, 0,  0,-1,-1, 0,
        1, 0, 1, 1, -1, 0, 1, 1,  1, 0,-1,-1, -1, 0,-1,-1,
        1, 1, 0, 1, -1, 1, 0, 1,  1,-1, 0,-1, -1,-1, 0,-1,
        0, 1, 1, 0,  0,-1, 1, 0, -1, 1, 0, 0,  0,-1,-1, 0,
    };

    static constexpr const inline int32_t PrimeX = 501125321;
    static constexpr const inline int32_t PrimeY = 1136930381;
    static constexpr const inline int32_t PrimeZ = 1338452629;
    static constexpr const inline int32_t PrimeT = 1720413743;

	class PerlinNoise : public Noise
	{
	public:
        PerlinNoise(const int32_t seed) : Noise(seed)
        {
        }

        inline float GenerateNoise(float x, float y) const
        {
            return GeneratePerlinNoise(seed, x, y);
        }

        inline float GenerateNoise(float x, float y, float z) const
        {
            return GeneratePerlinNoise(seed, x, y, z);
        }

        inline float GenerateNoise(float x, float y, float z, float t) const
        {
            return GeneratePerlinNoise(seed, x, y, z, t);
        }

	protected:
        static const inline float GeneratePerlinNoise(const int32_t seed, const float x, const float y)
        {
            int32_t x0 = IMath::FloorToInt(x);
            int32_t y0 = IMath::FloorToInt(y);

            float xd0 = (float)(x - x0);
            float yd0 = (float)(y - y0);
            float xd1 = xd0 - 1;
            float yd1 = yd0 - 1;

            float xs = IMath::InterpolationQuintic(xd0);
            float ys = IMath::InterpolationQuintic(yd0);

            x0 *= PrimeX;
            y0 *= PrimeY;
            int32_t x1 = x0 + PrimeX;
            int32_t y1 = y0 + PrimeY;

            float xf0 = IMath::Lerp(GradCoord(seed, x0, y0, xd0, yd0),
                                    GradCoord(seed, x1, y0, xd1, yd0), xs);
            float xf1 = IMath::Lerp(GradCoord(seed, x0, y1, xd0, yd1),
                                    GradCoord(seed, x1, y1, xd1, yd1), xs);

            return IMath::Lerp(xf0, xf1, ys) * 1.4247691104677813f;
        }

        static const inline float GeneratePerlinNoise(const int32_t seed, const float x, const float y, const float z)
        {
            int32_t x0 = IMath::FloorToInt(x);
            int32_t y0 = IMath::FloorToInt(y);
            int32_t z0 = IMath::FloorToInt(z);

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
            int32_t x1 = x0 + PrimeX;
            int32_t y1 = y0 + PrimeY;
            int32_t z1 = z0 + PrimeZ;

            float xf00 = IMath::Lerp(GradCoord(seed, x0, y0, z0, xd0, yd0, zd0),
                                     GradCoord(seed, x1, y0, z0, xd1, yd0, zd0), xs);
            float xf10 = IMath::Lerp(GradCoord(seed, x0, y1, z0, xd0, yd1, zd0),
                                     GradCoord(seed, x1, y1, z0, xd1, yd1, zd0), xs);
            float xf01 = IMath::Lerp(GradCoord(seed, x0, y0, z1, xd0, yd0, zd1),
                                     GradCoord(seed, x1, y0, z1, xd1, yd0, zd1), xs);
            float xf11 = IMath::Lerp(GradCoord(seed, x0, y1, z1, xd0, yd1, zd1),
                                     GradCoord(seed, x1, y1, z1, xd1, yd1, zd1), xs);

            float yf0 = IMath::Lerp(xf00, xf10, ys);
            float yf1 = IMath::Lerp(xf01, xf11, ys);

            return IMath::Lerp(yf0, yf1, zs) * 0.964921414852142333984375f;
        }

        static const inline float GeneratePerlinNoise(const int32_t seed, const float x, const float y, const float z, const float t)
        {
            int32_t x0 = IMath::FloorToInt(x);
            int32_t y0 = IMath::FloorToInt(y);
            int32_t z0 = IMath::FloorToInt(z);
            int32_t t0 = IMath::FloorToInt(t);

            float xd0 = (float)(x - x0);
            float yd0 = (float)(y - y0);
            float zd0 = (float)(z - z0);
            float td0 = (float)(t - t0);
            float xd1 = xd0 - 1;
            float yd1 = yd0 - 1;
            float zd1 = zd0 - 1;
            float td1 = td0 - 1;

            float xs = IMath::InterpolationQuintic(xd0);
            float ys = IMath::InterpolationQuintic(yd0);
            float zs = IMath::InterpolationQuintic(zd0);
            float ts = IMath::InterpolationQuintic(td0);

            x0 *= PrimeX;
            y0 *= PrimeY;
            z0 *= PrimeZ;
            t0 *= PrimeT;
            int32_t x1 = x0 + PrimeX;
            int32_t y1 = y0 + PrimeY;
            int32_t z1 = z0 + PrimeZ;
            int32_t t1 = t0 + PrimeT;

            float xf000 = IMath::Lerp(GradCoord(seed, x0, y0, z0, t0, xd0, yd0, zd0, td0),
                                      GradCoord(seed, x1, y0, z0, t0, xd1, yd0, zd0, td0), xs);
            float xf100 = IMath::Lerp(GradCoord(seed, x0, y1, z0, t0, xd0, yd1, zd0, td0),
                                      GradCoord(seed, x1, y1, z0, t0, xd1, yd1, zd0, td0), xs);
            float xf010 = IMath::Lerp(GradCoord(seed, x0, y0, z1, t0, xd0, yd0, zd1, td0),
                                      GradCoord(seed, x1, y0, z1, t0, xd1, yd0, zd1, td0), xs);
            float xf110 = IMath::Lerp(GradCoord(seed, x0, y1, z1, t0, xd0, yd1, zd1, td0),
                                      GradCoord(seed, x1, y1, z1, t0, xd1, yd1, zd1, td0), xs);
            float xf001 = IMath::Lerp(GradCoord(seed, x0, y0, z0, t1, xd0, yd0, zd0, td1),
                                      GradCoord(seed, x1, y0, z0, t1, xd1, yd0, zd0, td1), xs);
            float xf101 = IMath::Lerp(GradCoord(seed, x0, y1, z0, t1, xd0, yd1, zd0, td1),
                                      GradCoord(seed, x1, y1, z0, t1, xd1, yd1, zd0, td1), xs);
            float xf011 = IMath::Lerp(GradCoord(seed, x0, y0, z1, t1, xd0, yd0, zd1, td1),
                                      GradCoord(seed, x1, y0, z1, t1, xd1, yd0, zd1, td1), xs);
            float xf111 = IMath::Lerp(GradCoord(seed, x0, y1, z1, t1, xd0, yd1, zd1, td1),
                                      GradCoord(seed, x1, y1, z1, t1, xd1, yd1, zd1, td1), xs);

            float yf00 = IMath::Lerp(xf000, xf100, ys);
            float yf10 = IMath::Lerp(xf010, xf110, ys);
            float yf01 = IMath::Lerp(xf001, xf101, ys);
            float yf11 = IMath::Lerp(xf011, xf111, ys);

            float zf0 = IMath::Lerp(yf00, yf10, zs);
            float zf1 = IMath::Lerp(yf01, yf11, zs);

            return IMath::Lerp(zf0, zf1, ts) * 0.964921414852142333984375f;
        }

        static const inline int32_t Hash(const int32_t seed, const int32_t xPrimed, const int32_t yPrimed)
        {
            int32_t hash = seed ^ xPrimed ^ yPrimed;

            hash *= 0x27d4eb2d;
            return hash;
        }

        static const inline int32_t Hash(const int32_t seed, const int32_t xPrimed, const int32_t yPrimed, const int32_t zPrimed)
        {
            int32_t hash = seed ^ xPrimed ^ yPrimed ^ zPrimed;

            hash *= 0x27d4eb2d;
            return hash;
        }

        static const inline int32_t Hash(const int32_t seed, const int32_t xPrimed, const int32_t yPrimed, const int32_t zPrimed, const int32_t tPrimed)
        {
            int32_t hash = seed ^ xPrimed ^ yPrimed ^ zPrimed ^ tPrimed;

            hash *= 0x27d4eb2d;
            return hash;
        }

        static const inline float ValCoord(const int32_t seed, const int32_t xPrimed, const int32_t yPrimed)
        {
            int32_t hash = Hash(seed, xPrimed, yPrimed);

            hash *= hash;
            hash ^= hash << 19;
            return hash * (1 / 2147483648.0f);
        }

        static const inline float ValCoord(const int32_t seed, const int32_t xPrimed, const int32_t yPrimed, const int32_t zPrimed)
        {
            int32_t hash = Hash(seed, xPrimed, yPrimed, zPrimed);

            hash *= hash;
            hash ^= hash << 19;
            return hash * (1 / 2147483648.0f);
        }

        static const inline float ValCoord(const int32_t seed, const int32_t xPrimed, const int32_t yPrimed, const int32_t zPrimed, const int32_t tPrimed)
        {
            int32_t hash = Hash(seed, xPrimed, yPrimed, zPrimed, tPrimed);

            hash *= hash;
            hash ^= hash << 19;
            return hash * (1 / 2147483648.0f);
        }

        static const inline float GradCoord(const int32_t seed, const int32_t xPrimed, const int32_t yPrimed, const float xd, const float yd)
        {
            int32_t hash = Hash(seed, xPrimed, yPrimed);
            hash ^= hash >> 15;
            hash &= 127 << 1;

            float xg = Gradients2D[hash];
            float yg = Gradients2D[hash | 1];

            return xd * xg + yd * yg;
        }

        static const inline float GradCoord(const int32_t seed, const int32_t xPrimed, const int32_t yPrimed, const int32_t zPrimed, const float xd, const float yd, const float zd)
        {
            int32_t hash = Hash(seed, xPrimed, yPrimed, zPrimed);
            hash ^= hash >> 15;
            hash &= 63 << 2;

            float xg = Gradients3D[hash];
            float yg = Gradients3D[hash | 1];
            float zg = Gradients3D[hash | 2];

            return xd * xg + yd * yg + zd * zg;
        }

        static const inline float GradCoord(const int32_t seed, const int32_t xPrimed, const int32_t yPrimed, const int32_t zPrimed, const int32_t tPrimed, const float xd, const float yd, const float zd, const float td)
        {
            int32_t hash = Hash(seed, xPrimed, yPrimed, zPrimed, tPrimed);
            hash ^= hash >> 15;
            hash &= 31 << 2;

            float xg = Gradients4D[hash];
            float yg = Gradients4D[hash | 1];
            float zg = Gradients4D[hash | 2];
            float tg = Gradients4D[hash | 3];

            return xd * xg + yd * yg + zd * zg + td * tg;
        }
	};
}
