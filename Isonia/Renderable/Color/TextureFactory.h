#pragma once

// internal
#include "Color.h"
#include "../Texture.h"

// std
#include <array>

namespace Isonia::Renderable::Color
{
    class TextureFactory
    {
    private:
        static constexpr const uint32_t DEBUG_TEXTURE_WIDTH = 32;
        static constexpr const uint32_t DEBUG_TEXTURE_HEIGHT = 32;
        static constexpr const std::array<std::array<Color, DEBUG_TEXTURE_WIDTH>, DEBUG_TEXTURE_HEIGHT> CreateDebugTexture()
        {
            std::array<std::array<Color, DEBUG_TEXTURE_WIDTH>, DEBUG_TEXTURE_HEIGHT> DEBUG_TEXTURE;

            constexpr const Color BLACK = { 0, 0, 0, 255 };
            constexpr const Color TRANSPARENT = { 0, 0, 0, 0 };

            for (uint32_t i = 0; i < DEBUG_TEXTURE_WIDTH; ++i) {
                for (uint32_t j = 0; j < DEBUG_TEXTURE_HEIGHT; ++j) {
                    if ((i + j) % 2 == 0) {
                        DEBUG_TEXTURE[i][j] = BLACK;
                    }
                    else {
                        DEBUG_TEXTURE[i][j] = TRANSPARENT;
                    }
                }
            }
            return DEBUG_TEXTURE;
        }

    public:
        static Texture* CreateNullTexture(Pipeline::Device& device)
        {
            static constexpr const uint32_t TEXTURE_WIDTH = 2;
            static constexpr const uint32_t TEXTURE_HEIGHT = 2;
            static constexpr const Color NULL_TEXTURE[TEXTURE_WIDTH][TEXTURE_HEIGHT] =
            {
                { { 255, 0, 255 }, { 0, 0, 0 } },
                { { 255, 0, 255 }, { 0, 0, 0 } }
            };
            return Renderable::Texture::CreateTexture(device, NULL_TEXTURE, TEXTURE_WIDTH, TEXTURE_HEIGHT);
        }

        static Texture* CreateDebugTexture(Pipeline::Device& device)
        {
            static constexpr const std::array<std::array<Color, DEBUG_TEXTURE_WIDTH>, DEBUG_TEXTURE_HEIGHT> NULL_TEXTURE = CreateDebugTexture();
            return Renderable::Texture::CreateTexture(device, NULL_TEXTURE.data(), DEBUG_TEXTURE_WIDTH, DEBUG_TEXTURE_HEIGHT);
        }
    };
}
