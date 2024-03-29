#pragma once

// internal
#include "Color.h"
#include "../Texture.h"

namespace Isonia::Renderable::Color
{
    static constexpr const uint32_t PALETTE_LENGTH = 10;

    class PaletteFactory
    {
    public:
        static Texture* CreateGrassDayPalette(Pipeline::Device& device)
        {
            static constexpr const Color GRASS_DAY_PALETTE[PALETTE_LENGTH] =
            {
                { 56, 55, 77 },
                { 60, 63, 78 },
                { 74, 91, 88 },
                { 76, 105, 81 },
                { 104, 145, 101 },
                { 132, 191, 110 },
                { 157, 220, 114 },
                { 184, 232, 120 },
                { 195, 239, 126 },
                { 218, 250, 139 },
            };
            return Renderable::Texture::CreateTextureFromPalette(device, GRASS_DAY_PALETTE, PALETTE_LENGTH);
        }

        static Texture* CreateGrassNightPalette(Pipeline::Device& device)
        {
            static constexpr const Color GRASS_NIGHT_PALETTE[PALETTE_LENGTH] =
            {
                { 0, 14, 28 },
                { 1, 18, 32 },
                { 2, 22, 35 },
                { 4, 30, 45 },
                { 16, 46, 65 },
                { 24, 63, 90 },
                { 33, 89, 119 },
                { 51, 115, 145 },
                { 86, 154, 181 },
                { 102, 173, 198 },
            };
            return Renderable::Texture::CreateTextureFromPalette(device, GRASS_NIGHT_PALETTE, PALETTE_LENGTH);
        }

        static Texture* CreateWaterDayPalette(Pipeline::Device& device)
        {
            static constexpr const Color WATER_DAY_PALETTE[PALETTE_LENGTH] =
            {
                { 187, 222, 229 },
                { 178, 221, 230 },
                { 157, 214, 226 },
                { 133, 209, 226 },
                { 88, 190, 219 },
                { 82, 190, 222 },
                { 64, 177, 212 },
                { 44, 146, 184 },
                { 15, 101, 138 },
                { 16, 81, 123 },
            };
            return Renderable::Texture::CreateTextureFromPalette(device, WATER_DAY_PALETTE, PALETTE_LENGTH);
        }
    };
}
