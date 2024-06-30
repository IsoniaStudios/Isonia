#include "Noise.h"

namespace Isonia::Noise
{
    CurlNoise::CurlNoise(const VirtualNoise* base_noise) : VirtualWarpNoise(), base_noise(base_noise) { }

    inline void CurlNoise::transformCoordinate(float* x, float* y) const
    {
        const float x1 = base_noise->generateNoise(*x, *y + Math::epsilon);
        const float x2 = base_noise->generateNoise(*x, *y - Math::epsilon);
        const float dx = (x1 - x2) / Math::two_epsilon;

        const float y1 = base_noise->generateNoise(*x + Math::epsilon, *y);
        const float y2 = base_noise->generateNoise(*x - Math::epsilon, *y);
        const float dy = -(y1 - y2) / Math::two_epsilon;

        *x = dx;
        *y = dy;
    }

    inline void CurlNoise::transformCoordinate(float* x, float* y, float* z) const
    {
        const float noise_offset = 1000.0f;

        const float x1y = base_noise->generateNoise(*x + Math::epsilon, *y, *z + noise_offset);
        const float x1z = base_noise->generateNoise(*x + Math::epsilon + noise_offset, *y, *z);
        const float x2y = base_noise->generateNoise(*x - Math::epsilon, *y, *z + noise_offset);
        const float x2z = base_noise->generateNoise(*x - Math::epsilon + noise_offset, *y, *z);
        const float dxy = x1y - x2y;
        const float dxz = x1z - x2z;

        const float y1x = base_noise->generateNoise(*x, *y + Math::epsilon, *z);
        const float y1z = base_noise->generateNoise(*x + noise_offset, *y + Math::epsilon, *z);
        const float y2x = base_noise->generateNoise(*x, *y - Math::epsilon, *z);
        const float y2z = base_noise->generateNoise(*x + noise_offset, *y - Math::epsilon, *z);
        const float dyx = y1x - y2x;
        const float dyz = y1z - y2z;

        const float z1x = base_noise->generateNoise(*x, *y, *z + Math::epsilon);
        const float z1y = base_noise->generateNoise(*x, *y, *z + Math::epsilon + noise_offset);
        const float z2x = base_noise->generateNoise(*x, *y, *z - Math::epsilon);
        const float z2y = base_noise->generateNoise(*x, *y, *z - Math::epsilon + noise_offset);
        const float dzx = z1x - z2x;
        const float dzy = z1y - z2y;

        *x = (dyz - dzy) / (Math::two_epsilon);
        *y = (dzx - dxz) / (Math::two_epsilon);
        *z = (dxy - dyx) / (Math::two_epsilon);
    }

    inline void CurlNoise::transformCoordinate(float* x, float* y, float* z, float* w) const
    {
        const float noise_offset = 1000.0f;

        const float x1y = base_noise->generateNoise(*x + Math::epsilon, *y, *z, *w + noise_offset);
        const float x1z = base_noise->generateNoise(*x + Math::epsilon, *y, *z + noise_offset, *w);
        const float x1w = base_noise->generateNoise(*x + Math::epsilon, *y + noise_offset, *z, *w);
        const float x2y = base_noise->generateNoise(*x - Math::epsilon, *y, *z, *w + noise_offset);
        const float x2z = base_noise->generateNoise(*x - Math::epsilon, *y, *z + noise_offset, *w);
        const float x2w = base_noise->generateNoise(*x - Math::epsilon, *y + noise_offset, *z, *w);
        const float dxy = x1y - x2y;
        const float dxz = x1z - x2z;
        const float dxw = x1w - x2w;

        const float y1x = base_noise->generateNoise(*x, *y + Math::epsilon, *z, *w);
        const float y1z = base_noise->generateNoise(*x + noise_offset, *y + Math::epsilon, *z, *w);
        const float y1w = base_noise->generateNoise(*x, *y + Math::epsilon, *z + noise_offset, *w);
        const float y2x = base_noise->generateNoise(*x, *y - Math::epsilon, *z, *w);
        const float y2z = base_noise->generateNoise(*x + noise_offset, *y - Math::epsilon, *z, *w);
        const float y2w = base_noise->generateNoise(*x, *y - Math::epsilon, *z + noise_offset, *w);
        const float dyx = y1x - y2x;
        const float dyz = y1z - y2z;
        const float dyw = y1w - y2w;

        const float z1x = base_noise->generateNoise(*x, *y, *z + Math::epsilon, *w);
        const float z1y = base_noise->generateNoise(*x + noise_offset, *y, *z + Math::epsilon, *w);
        const float z1w = base_noise->generateNoise(*x, *y + noise_offset, *z + Math::epsilon, *w);
        const float z2x = base_noise->generateNoise(*x, *y, *z - Math::epsilon, *w);
        const float z2y = base_noise->generateNoise(*x + noise_offset, *y, *z - Math::epsilon, *w);
        const float z2w = base_noise->generateNoise(*x, *y + noise_offset, *z - Math::epsilon, *w);
        const float dzx = z1x - z2x;
        const float dzy = z1y - z2y;
        const float dzw = z1w - z2w;

        const float w1x = base_noise->generateNoise(*x, *y, *z, *w + Math::epsilon);
        const float w1y = base_noise->generateNoise(*x + noise_offset, *y, *z, *w + Math::epsilon);
        const float w1z = base_noise->generateNoise(*x, *y + noise_offset, *z, *w + Math::epsilon);
        const float w2x = base_noise->generateNoise(*x, *y, *z, *w - Math::epsilon);
        const float w2y = base_noise->generateNoise(*x + noise_offset, *y, *z, *w - Math::epsilon);
        const float w2z = base_noise->generateNoise(*x, *y + noise_offset, *z, *w - Math::epsilon);
        const float wdx = w1x - w2x;
        const float wdy = w1y - w2y;
        const float wdz = w1z - w2z;

        *x = (dyz - dzy) / (Math::two_epsilon);
        *y = (dzx - dxz) / (Math::two_epsilon);
        *z = (dxw - dyw) / (Math::two_epsilon);
        *w = (dxy - wdz) / (Math::two_epsilon);
    }
}
