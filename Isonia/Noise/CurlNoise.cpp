#include "Noise.h"

namespace Isonia::Noise
{
    CurlNoise::CurlNoise(const VirtualNoise* base_noise, const VirtualWarpNoise* base_warp_noise)
        : VirtualWarpNoise(), base_noise(base_noise), base_warp_noise(base_warp_noise)
    {
    }

    void CurlNoise::transformCoordinate(float* x, float* y) const
    {
        base_warp_noise->transformCoordinate(x, y);

        const float x1 = base_noise->generateNoise(*x, *y + Math::float_delta);
        const float x2 = base_noise->generateNoise(*x, *y - Math::float_delta);
        const float dx = (x1 - x2) / Math::two_float_delta;

        const float y1 = base_noise->generateNoise(*x + Math::float_delta, *y);
        const float y2 = base_noise->generateNoise(*x - Math::float_delta, *y);
        const float dy = -(y1 - y2) / Math::two_float_delta;

        *x = dx;
        *y = dy;

        Math::float2Normalize(x, y);
    }

    void CurlNoise::transformCoordinate(float* x, float* y, float* z) const
    {
        base_warp_noise->transformCoordinate(x, y, z);

        const float noise_offset = 1'000.0f;

        const float x1y = base_noise->generateNoise(*x + Math::float_delta, *y, *z + noise_offset);
        const float x1z = base_noise->generateNoise(*x + Math::float_delta + noise_offset, *y, *z);
        const float x2y = base_noise->generateNoise(*x - Math::float_delta, *y, *z + noise_offset);
        const float x2z = base_noise->generateNoise(*x - Math::float_delta + noise_offset, *y, *z);
        const float dxy = x1y - x2y;
        const float dxz = x1z - x2z;

        const float y1x = base_noise->generateNoise(*x, *y + Math::float_delta, *z);
        const float y1z = base_noise->generateNoise(*x + noise_offset, *y + Math::float_delta, *z);
        const float y2x = base_noise->generateNoise(*x, *y - Math::float_delta, *z);
        const float y2z = base_noise->generateNoise(*x + noise_offset, *y - Math::float_delta, *z);
        const float dyx = y1x - y2x;
        const float dyz = y1z - y2z;

        const float z1x = base_noise->generateNoise(*x, *y, *z + Math::float_delta);
        const float z1y = base_noise->generateNoise(*x, *y, *z + Math::float_delta + noise_offset);
        const float z2x = base_noise->generateNoise(*x, *y, *z - Math::float_delta);
        const float z2y = base_noise->generateNoise(*x, *y, *z - Math::float_delta + noise_offset);
        const float dzx = z1x - z2x;
        const float dzy = z1y - z2y;

        *x = (dyz - dzy) / (Math::two_float_delta);
        *y = (dzx - dxz) / (Math::two_float_delta);
        *z = (dxy - dyx) / (Math::two_float_delta);

        Math::float3Normalize(x, y, z);
    }

    void CurlNoise::transformCoordinate(float* x, float* y, float* z, float* w) const
    {
        base_warp_noise->transformCoordinate(x, y, z, w);

        const float noise_offset = 1'000.0f;

        const float x1y = base_noise->generateNoise(*x + Math::float_delta, *y, *z, *w + noise_offset);
        const float x1z = base_noise->generateNoise(*x + Math::float_delta, *y, *z + noise_offset, *w);
        const float x1w = base_noise->generateNoise(*x + Math::float_delta, *y + noise_offset, *z, *w);
        const float x2y = base_noise->generateNoise(*x - Math::float_delta, *y, *z, *w + noise_offset);
        const float x2z = base_noise->generateNoise(*x - Math::float_delta, *y, *z + noise_offset, *w);
        const float x2w = base_noise->generateNoise(*x - Math::float_delta, *y + noise_offset, *z, *w);
        const float dxy = x1y - x2y;
        const float dxz = x1z - x2z;
        const float dxw = x1w - x2w;

        const float y1x = base_noise->generateNoise(*x, *y + Math::float_delta, *z, *w);
        const float y1z = base_noise->generateNoise(*x + noise_offset, *y + Math::float_delta, *z, *w);
        const float y1w = base_noise->generateNoise(*x, *y + Math::float_delta, *z + noise_offset, *w);
        const float y2x = base_noise->generateNoise(*x, *y - Math::float_delta, *z, *w);
        const float y2z = base_noise->generateNoise(*x + noise_offset, *y - Math::float_delta, *z, *w);
        const float y2w = base_noise->generateNoise(*x, *y - Math::float_delta, *z + noise_offset, *w);
        const float dyx = y1x - y2x;
        const float dyz = y1z - y2z;
        const float dyw = y1w - y2w;

        const float z1x = base_noise->generateNoise(*x, *y, *z + Math::float_delta, *w);
        const float z1y = base_noise->generateNoise(*x + noise_offset, *y, *z + Math::float_delta, *w);
        const float z1w = base_noise->generateNoise(*x, *y + noise_offset, *z + Math::float_delta, *w);
        const float z2x = base_noise->generateNoise(*x, *y, *z - Math::float_delta, *w);
        const float z2y = base_noise->generateNoise(*x + noise_offset, *y, *z - Math::float_delta, *w);
        const float z2w = base_noise->generateNoise(*x, *y + noise_offset, *z - Math::float_delta, *w);
        const float dzx = z1x - z2x;
        const float dzy = z1y - z2y;
        const float dzw = z1w - z2w;

        const float w1x = base_noise->generateNoise(*x, *y, *z, *w + Math::float_delta);
        const float w1y = base_noise->generateNoise(*x + noise_offset, *y, *z, *w + Math::float_delta);
        const float w1z = base_noise->generateNoise(*x, *y + noise_offset, *z, *w + Math::float_delta);
        const float w2x = base_noise->generateNoise(*x, *y, *z, *w - Math::float_delta);
        const float w2y = base_noise->generateNoise(*x + noise_offset, *y, *z, *w - Math::float_delta);
        const float w2z = base_noise->generateNoise(*x, *y + noise_offset, *z, *w - Math::float_delta);
        const float wdx = w1x - w2x;
        const float wdy = w1y - w2y;
        const float wdz = w1z - w2z;

        *x = (dyz - dzy) / (Math::two_float_delta);
        *y = (dzx - dxz) / (Math::two_float_delta);
        *z = (dxw - dyw) / (Math::two_float_delta);
        *w = (dxy - wdz) / (Math::two_float_delta);

        Math::float4Normalize(x, y, z, w);
    }
}
