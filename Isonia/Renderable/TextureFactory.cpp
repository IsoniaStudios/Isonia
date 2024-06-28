// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	extern Texture* createNullTexture(Pipeline::Device* device)
	{
		constexpr const unsigned int texture_width = 2u;
		constexpr const unsigned int texture_height = 2u;
		constexpr const Color NULL_TEXTURE[texture_width][texture_height] =
		{
			{ { 255, 0, 255 }, { 0 } },
			{ { 0 }, { 255, 0, 255 } }
		};
		return Renderable::Texture::createTexture(device, NULL_TEXTURE, texture_width, texture_height);
	}

	extern Texture* createDebugTexture(Pipeline::Device* device)
	{
		constexpr const unsigned int texture_width = 32u;
		constexpr const unsigned int texture_height = 32u;

		Color texture[texture_width * texture_height];

		constexpr const Color black = { 0, 0, 0, 255 };
		constexpr const Color transparent = { 0, 0, 0, 0 };

		for (unsigned int i = 0u; i < texture_width; i++)
		{
			for (unsigned int j = 0u; j < texture_height; j++)
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

	static Color GrassSubTextureFiller(const unsigned int atlas_height, const unsigned int atlas_width, const unsigned int texture_height, const unsigned int texture_width)
	{
		bool hit_grass = texture_width == 1;
		if (hit_grass)
		{
			return { 255 };
		}
		else
		{
			return { 0 };
		}
	}
	extern Texture* createGrassTexture(Pipeline::Device* device)
	{
		const unsigned int atlas_height = 8u;
		const unsigned int atlas_width = 8u;
		const unsigned int texture_height = 16u;
		const unsigned int texture_width = 16u;

		return createTextureAtlas(device, GrassSubTextureFiller, atlas_height, atlas_width, texture_height, texture_width);
	}

	extern Texture* createTextureAtlas(Pipeline::Device* device, SubTextureFiller sub_texture_filler, const unsigned int atlas_height, const unsigned int atlas_width, const unsigned int texture_height, const unsigned int texture_width)
	{
		const unsigned int texture_atlas_height = atlas_height * texture_height;
		const unsigned int texture_atlas_width = atlas_width * texture_width;

		Color* texture = new Color[texture_atlas_height * texture_atlas_width];

		for (unsigned int a_h = 0u; a_h < atlas_height; a_h++)
		{
			for (unsigned int t_h = 0u; t_h < texture_height; t_h++)
			{
				for (unsigned int a_w = 0u; a_w < atlas_width; a_w++)
				{
					for (unsigned int t_w = 0u; t_w < texture_width; t_w++)
					{
						const unsigned int index = (texture_height * a_h + t_h) * atlas_width * texture_width + (texture_width * a_w + t_w);
						texture[index] = sub_texture_filler(a_h, a_w, t_h, t_w); // its lying?!?!
					}
				}
			}
		}
		return Texture::createTexture(device, texture, texture_atlas_width, texture_atlas_height);
	}
}
