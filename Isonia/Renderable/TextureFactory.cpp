// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	extern inline Texture* createNullTexture(Pipeline::Device* device)
	{
		constexpr const unsigned int texture_width = 2;
		constexpr const unsigned int texture_height = 2;
		constexpr const Color NULL_TEXTURE[texture_width][texture_height] =
		{
			{ { 255, 0, 255 }, { 0, 0, 0 } },
			{ { 255, 0, 255 }, { 0, 0, 0 } }
		};
		return Renderable::Texture::createTexture(device, NULL_TEXTURE, texture_width, texture_height);
	}

	extern inline Texture* createDebugTexture(Pipeline::Device* device)
	{
        constexpr const unsigned int texture_width = 32;
        constexpr const unsigned int texture_height = 32;

        Color texture[texture_width * texture_height];

        constexpr const Color black = { 0, 0, 0, 255 };
        constexpr const Color transparent = { 0, 0, 0, 0 };

        for (unsigned int i = 0; i < texture_width; ++i)
        {
            for (unsigned int j = 0; j < texture_height; ++j)
            {
                if ((i + j) % 2 == 0)
                {
                    texture[i * texture_height + j] = black;
                }
                else
                {
                    texture[i * texture_height + j] = transparent;
                }
            }
        }
        return Texture::createTexture(device, texture, texture_width, texture_height);
	}
}
