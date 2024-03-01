#pragma once

namespace Isonia::Noise
{
    // https://stackoverflow.com/questions/5281261/generating-a-normal-map-from-a-height-map
    inline static float sample_pixel(float* heightMap, uint32_t width, uint32_t z, uint32_t x)
    {
        return heightMap[z * width + x];
    }

    inline static float pixel_diff(float* heightMap, uint32_t width, uint32_t z1, uint32_t z2, uint32_t x1, uint32_t x2)
    {
        return sample_pixel(heightMap, width, z2, x2) - sample_pixel(heightMap, width, z1, x1);
    }

    inline static float row_dh(float* heightMap, uint32_t width, uint32_t z, uint32_t x)
    {
        if (z == 1)
            return 0;
        if (z == 0)
            return -pixel_diff(heightMap, width, 0, 1, x, x);
        if (z == width - 1)
            return -pixel_diff(heightMap, width, width - 2, width - 1, x, x);
        return -pixel_diff(heightMap, width, z - 1, z + 1, x, x);
    }

    inline static float row_dh_wrap(float* heightMap, uint32_t width, uint32_t z, uint32_t x)
    {
        if (z == 1)
            return 0.0f;
        if (z == 0)
            return -pixel_diff(heightMap, width, 1, width - 1, x, x);
        if (z == width - 1)
            return -pixel_diff(heightMap, width, width - 2, 0, x, x);
        return -pixel_diff(heightMap, width, z - 1, z + 1, x, x);
    }

    inline static float col_dh(float* heightMap, uint32_t width, uint32_t height, uint32_t z, uint32_t x)
    {
        if (height == 1)
            return 0.0f;
        if (x == 0)
            return pixel_diff(heightMap, width, z, z, 0, 1);
        if (x == height - 1)
            return pixel_diff(heightMap, width, z, z, height - 2, height - 1);
        return pixel_diff(heightMap, width, z, z, x - 1, x + 1);
    }

    inline static float col_dh_wrap(float* heightMap, uint32_t width, uint32_t height, uint32_t z, uint32_t x)
    {
        if (height == 1)
            return 0.0f;
        if (x == 0)
            return pixel_diff(heightMap, width, z, z, 1, height - 1);
        if (x == height - 1)
            return pixel_diff(heightMap, width, z, z, height - 2, 0);
        return pixel_diff(heightMap, width, z, z, x - 1, x + 1);
    }

    inline static inline glm::vec3 ComputeSmoothNormalFrom9AndWrap(float* heightMap, uint32_t width, uint32_t height, float scale, uint32_t z, uint32_t x)
    {
        glm::vec3 nv;

        float dh;
        float div = 5.0;
        dh = row_dh_wrap(heightMap, width, z, x) * 2
            + (x == 0) ? row_dh_wrap(heightMap, width, z, height - 1) : row_dh_wrap(heightMap, width, z, x - 1)
            + (x == height - 1) ? row_dh_wrap(heightMap, width, z, 0) : row_dh_wrap(heightMap, width, z, x + 1);
        nv.x = scale * dh / div;
        dh = col_dh_wrap(heightMap, width, height, z, x) * 2
            + (z == 0) ? col_dh_wrap(heightMap, width, height, width - 1, x) : col_dh_wrap(heightMap, width, height, z - 1, x)
            + (z == width - 1) ? col_dh_wrap(heightMap, width, height, 0, x) : col_dh_wrap(heightMap, width, height, z + 1, x);
        nv.y = scale * dh / div;

        div = sqrt(nv.x * nv.x + nv.y * nv.y + 1.0);
        nv.x /= div;
        nv.y /= div;
        nv.z = 1.0 / div;

        return nv;
    }

    inline static inline glm::vec3 ComputeSmoothNormalFrom9(float* heightMap, uint32_t width, uint32_t height, float scale, uint32_t z, uint32_t x)
    {
        glm::vec3 nv;

        float dh;
        float div = 5.0;

        dh = row_dh(heightMap, width, z, x) * 2;
        if (x == 0)
            div -= 1.0;
        else
            dh += row_dh(heightMap, width, z, x - 1);

        if (x == height - 1)
            div -= 1.0;
        else
            dh += row_dh(heightMap, width, z, x + 1);
        nv.x = scale * dh / div;

        div = 5.0;
        dh = col_dh(heightMap, width, height, z, x) * 2;
        if (z == 0)
            div -= 1.0;
        else
            dh += col_dh(heightMap, width, height, z - 1, x);
        if (z == width - 1)
            div -= 1.0;
        else
            dh += col_dh(heightMap, width, height, z + 1, x);

        nv.y = scale * dh / div;

        div = sqrt(nv.x * nv.x + nv.y * nv.y + 1.0);
        nv.x /= div;
        nv.y /= div;
        nv.z = 1.0 / div;

        return nv;
    }
}
