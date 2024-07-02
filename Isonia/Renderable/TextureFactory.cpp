// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	extern Texture* createNullTexture(Pipeline::Device* device)
	{
		const unsigned int texture_width = 2u;
		const unsigned int texture_height = 2u;
		const Color NULL_TEXTURE[texture_width][texture_height] =
		{
			{ { 255, 0, 255 }, { 0 } },
			{ { 0 }, { 255, 0, 255 } }
		};
		return Renderable::Texture::createTexture(device, NULL_TEXTURE, texture_width, texture_height);
	}

	extern Texture* createDebugTexture(Pipeline::Device* device)
	{
		const unsigned int texture_width = 32u;
		const unsigned int texture_height = 32u;

		Color texture[texture_width * texture_height];

		const Color black = { 0, 0, 0, 255 };
		const Color transparent = { 0, 0, 0, 0 };

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

	static const unsigned int grass_atlas_height = 9u;
	static const unsigned int grass_atlas_width = 9u;
	static const unsigned int grass_texture_height = 16u;
	static const unsigned int grass_texture_width = 16u;
	static const unsigned int grass_texture_msaa = 4u;
	static struct Grass
	{
		float root_position;
		float height;
		float radius;
		float radius_given_height(float t_h) const
		{
			const float h = 1.0f - t_h;
			if (h > height)
			{
				return 0.0f;
			}
			const float s = Math::sigmoidf(0.75f, 10.0f, (h * 12.0f) / height);
			return s * radius;
		}
	};
	static inline Color grassSubTextureFiller(const unsigned int a_h, const unsigned int a_w, const unsigned int t_h, const unsigned int t_w)
	{
		// parameters
		const float parameter_force = 0.01f;
		const unsigned int parameter_blade_count = 3u;
		const Grass blades[] = {
			{0.33f, 0.45f, 0.0425f},
			{0.50f, 0.50f, 0.0450f},
			{0.66f, 0.40f, 0.0400f},
		};

		const float ah = static_cast<float>(static_cast<int>(a_h) - static_cast<int>(grass_atlas_height / 2u)) / static_cast<float>(grass_atlas_height);
		const float aw = static_cast<float>(static_cast<int>(a_w) - static_cast<int>(grass_atlas_width / 2u)) / static_cast<float>(grass_atlas_width);

		const float th = static_cast<float>(t_h) / static_cast<float>(grass_texture_height);
		const float tw = static_cast<float>(t_w) / static_cast<float>(grass_texture_width);

		for (unsigned int i = 0; i < parameter_blade_count; i++)
		{
			const float wind_z = Math::absf(ah * 0.5f);
			const float radius = blades[i].radius_given_height(th - wind_z);
			if (radius == 0)
			{
				continue;
			}
			const float force = (parameter_force / th) * 100.0f;
			const float structural_force = parameter_force / radius;
			const float final_force = (force + structural_force) * Math::maxf(Math::sigmoidf(25.0f, 0.85f, th) - 0.25f, 0.0f);
			const float wind_x = aw * final_force;

			const float base_blade_position = blades[i].root_position;
			if (Math::absf(tw - base_blade_position - wind_x) < radius)
			{
				return { 255 };
			}
		}
		return { 0, 0, 0 };
	}
	extern Texture* createGrassTexture(Pipeline::Device* device)
	{
		return createTextureAtlas(device, grassSubTextureFiller, grass_atlas_height, grass_atlas_width, grass_texture_height, grass_texture_width);
	}

	extern Color* createTextureAtlas(SubTextureFiller sub_texture_filler, const unsigned int atlas_height, const unsigned int atlas_width, const unsigned int texture_height, const unsigned int texture_width)
	{
		const unsigned int texture_atlas_height = atlas_height * texture_height;
		const unsigned int texture_atlas_width = atlas_width * texture_width;
		const unsigned int texture_atlas_size = texture_atlas_height * texture_atlas_width;

		Color* texture_atlas = new Color[texture_atlas_size];
		for (unsigned int a_h = 0u; a_h < atlas_height; a_h++)
		{
			const unsigned int base_i_a_h = texture_height * texture_atlas_width * a_h;
			for (unsigned int a_w = 0u; a_w < atlas_width; a_w++)
			{
				const unsigned int base_i_a_w = texture_width * a_w;
				for (unsigned int t_h = 0u; t_h < texture_height; t_h++)
				{
					const unsigned int base_i_t_h = t_h * texture_atlas_width;
					for (unsigned int t_w = 0u; t_w < texture_width; t_w++)
					{
						const unsigned int index = base_i_a_h + base_i_t_h + base_i_a_w + t_w;
						texture_atlas[index] = sub_texture_filler(a_h, a_w, t_h, t_w);
					}
				}
			}
		}
		/* cleaner but slower, a lot slower
		for (unsigned int index = 0; index < texture_atlas_size; index++)
		{
			const unsigned int atlas_row = index / texture_atlas_width;
			const unsigned int atlas_col = index % texture_atlas_width;

			const unsigned int a_h = atlas_row / texture_height;
			const unsigned int t_h = atlas_row % texture_height;
			const unsigned int a_w = atlas_col / texture_width;
			const unsigned int t_w = atlas_col % texture_width;

			texture_atlas[index] = sub_texture_filler(a_h, a_w, t_h, t_w);
		}
		*/
		return texture_atlas;
	}
	extern Texture* createTextureAtlas(Pipeline::Device* device, SubTextureFiller sub_texture_filler, const unsigned int atlas_height, const unsigned int atlas_width, const unsigned int texture_height, const unsigned int texture_width)
	{
		Color* texture_atlas = createTextureAtlas(sub_texture_filler, atlas_height, atlas_width, texture_height, texture_width);
		Texture* texture = Texture::createTexture(device, texture_atlas, atlas_width * texture_width, atlas_height * texture_height);
		delete texture_atlas;
		return texture;
	}
	extern Texture* createTextureAtlasMSAA(Pipeline::Device* device, SubTextureFiller sub_texture_filler, const unsigned int atlas_height, const unsigned int atlas_width, const unsigned int texture_height, const unsigned int texture_width, const unsigned int msaa)
	{
		Color* msaa_texture_atlas = createTextureAtlas(sub_texture_filler, atlas_height, atlas_width, texture_height * msaa, texture_width * msaa);

		const unsigned int texture_atlas_height = atlas_height * texture_height;
		const unsigned int texture_atlas_width = atlas_width * texture_width;
		const unsigned int texture_atlas_size = texture_atlas_height * texture_atlas_width;
		Color* texture_atlas = new Color[texture_atlas_size];

		for (unsigned int a_h = 0u; a_h < atlas_height; a_h++)
		{
			const unsigned int base_i_a_h = texture_height * texture_atlas_width * a_h;
			for (unsigned int a_w = 0u; a_w < atlas_width; a_w++)
			{
				const unsigned int base_i_a_w = texture_width * a_w;
				for (unsigned int t_h = 0u; t_h < texture_height; t_h++)
				{
					const unsigned int base_i_t_h = t_h * texture_atlas_width;
					for (unsigned int t_w = 0u; t_w < texture_width; t_w++)
					{
						unsigned int totalR = 0u;
						unsigned int totalG = 0u;
						unsigned int totalB = 0u;
						unsigned int totalA = 0u;

						for (unsigned int m_h = 0u; m_h < msaa; m_h++)
						{
							for (unsigned int m_w = 0u; m_w < msaa; m_w++)
							{
								const unsigned int msaa_index =
									(texture_height * atlas_width * texture_width * a_h * msaa * msaa) +
									((t_h * msaa + m_h) * atlas_width * texture_width * msaa) +
									(texture_width * a_w * msaa) +
									(t_w * msaa + m_w);
								Color sample = msaa_texture_atlas[msaa_index];
								totalR += sample.r;
								totalG += sample.g;
								totalB += sample.b;
								totalA += sample.a;
							}
						}

						unsigned int rgba_divisor = msaa * msaa;
						unsigned char avgR = static_cast<unsigned char>(totalR / rgba_divisor);
						unsigned char avgG = static_cast<unsigned char>(totalG / rgba_divisor);
						unsigned char avgB = static_cast<unsigned char>(totalB / rgba_divisor);
						unsigned char avgA = static_cast<unsigned char>(totalA / rgba_divisor);

						const unsigned int index = base_i_a_h + base_i_t_h + base_i_a_w + t_w;
						texture_atlas[index] = Color(avgR, avgG, avgB, avgA);
					}
				}
			}
		}
		Texture* texture = Texture::createTexture(device, texture_atlas, texture_atlas_width, texture_atlas_height);
		delete msaa_texture_atlas;
		delete texture_atlas;
		return texture;
	}
}
