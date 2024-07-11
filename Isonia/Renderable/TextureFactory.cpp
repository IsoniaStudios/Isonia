// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	extern Texture* createNullTexture(Pipeline::Device* device)
	{
		const unsigned int texture_width = 2u;
		const unsigned int texture_height = 2u;
		const Color null_texture[texture_width][texture_height] =
		{
			{ { 255, 0, 255 }, { 0 } },
			{ { 0 }, { 255, 0, 255 } }
		};
		return new Texture(device, null_texture, texture_width, texture_height, VK_FORMAT_R8G8B8A8_SRGB);
	}

	extern Texture* createDebugTexture(Pipeline::Device* device)
	{
		const unsigned int texture_width = 32u;
		const unsigned int texture_height = 32u;

		Color debug_texture[texture_width * texture_height];

		const Color black = { 0, 0, 0, 255 };
		const Color transparent = { 0, 0, 0, 0 };

		for (unsigned int i = 0u; i < texture_width; i++)
		{
			for (unsigned int j = 0u; j < texture_height; j++)
			{
				if ((i + j) % 2 == 0)
				{
					debug_texture[i * texture_height + j] = black;
				}
				else
				{
					debug_texture[i * texture_height + j] = transparent;
				}
			}
		}
		return new Texture(device, debug_texture, texture_width, texture_height, VK_FORMAT_R8G8B8A8_SRGB);
	}

	TextureFactory::TextureFactory(const unsigned int texture_height, const unsigned int texture_width, const unsigned char stride)
		: texture_height(texture_height), texture_width(texture_width), stride(stride)
	{
	}
	inline unsigned int TextureFactory::getTextureHeight() const
	{
		return texture_height;
	}
	inline unsigned int TextureFactory::getTextureWidth() const
	{
		return texture_width;
	}
	inline unsigned int TextureFactory::getTextureSize() const
	{
		return getTextureHeight() * getTextureWidth() * stride;
	}
	Texture* TextureFactory::instantiateTexture(Pipeline::Device* device, const VkFormat format, const VkFilter filter, const VkSamplerAddressMode address_mode) const
	{
		void* texture_data = createTexture();
		Texture* texture = new Texture(device, texture_data, getTextureHeight(), getTextureWidth(), format, filter, address_mode);
		delete texture_data;
		return texture;
	}
	void* TextureFactory::createTexture() const
	{
		// NOTE: using modulus is a lot cleaner but a lot slower
		const unsigned int texture_size = getTextureSize();
		void* texture = malloc(texture_size);
		for (unsigned int t_h = 0u; t_h < texture_height; t_h++)
		{
			const unsigned int base_i_t_h = t_h * texture_width;
			for (unsigned int t_w = 0u; t_w < texture_width; t_w++)
			{
				const unsigned int index = base_i_t_h + t_w;
				textureFiller(texture, index, t_h, t_w);
			}
		}
		return texture;
	}

	TextureAtlasFactory::TextureAtlasFactory(const unsigned int atlas_height, const unsigned int atlas_width, const unsigned int texture_height, const unsigned int texture_width, const unsigned char stride)
		: TextureFactory(texture_height, texture_width, stride),  atlas_height(atlas_height), atlas_width(atlas_width)
	{
	}
	inline unsigned int TextureAtlasFactory::getTextureHeight() const
	{
		return texture_height * atlas_height;
	}
	inline unsigned int TextureAtlasFactory::getTextureWidth() const
	{
		return texture_width * atlas_width;
	}
	void* TextureAtlasFactory::createTexture() const
	{
		// NOTE: using modulus is a lot cleaner but a lot slower
		const unsigned int texture_atlas_width = getTextureWidth();
		const unsigned int texture_size = getTextureSize();
		void* texture = malloc(texture_size);
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
						textureFiller(texture, index, a_h, a_w, t_h, t_w);
					}
				}
			}
		}
		return texture;
	}

	Noise4DTextureFactory::Noise4DTextureFactory(const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const unsigned int texture_height, const unsigned int texture_width, const unsigned char stride)
		: TextureFactory(texture_height, texture_width, stride), warp_noise(warp_noise), noise(noise)
	{
	}
	inline void Noise4DTextureFactory::textureFiller(void* memory, const unsigned int index, const unsigned int t_h, const unsigned int t_w) const
	{
		unsigned char* value = static_cast<unsigned char*>(memory) + index;

		const float s = t_h / static_cast<float>(getTextureHeight());
		const float t = t_w / static_cast<float>(getTextureWidth());

		float nx = Math::cosf(s * Math::two_pi) / (Math::two_pi);
		float ny = Math::cosf(t * Math::two_pi) / (Math::two_pi);
		float nz = Math::sinf(s * Math::two_pi) / (Math::two_pi);
		float nt = Math::sinf(t * Math::two_pi) / (Math::two_pi);

		warp_noise->transformCoordinate(&nx, &ny, &nz, &nt);
		const float noise_value = noise->generateNoise(nx, ny, nz, nt);
		const float pushed_value = (noise_value + 1.0f) * 0.5f;
		*value = static_cast<unsigned char>(pushed_value * 255.0f);
	}

	WarpNoiseTextureFactory::WarpNoiseTextureFactory(const Noise::VirtualWarpNoise* warp_noise, const unsigned int texture_height, const unsigned int texture_width, const unsigned char stride)
		: TextureFactory(texture_height, texture_width, stride), warp_noise(warp_noise)
	{
	}
	inline void WarpNoiseTextureFactory::textureFiller(void* memory, const unsigned int index, const unsigned int t_h, const unsigned int t_w) const
	{
		float s = static_cast<float>(t_h) / getTextureHeight();
		float t = static_cast<float>(t_w) / getTextureWidth();

		warp_noise->transformCoordinate(&s, &t);

		const char sc = static_cast<char>(s * 127.0f);
		const char tc = static_cast<char>(t * 127.0f);

		unsigned char* value = static_cast<unsigned char*>(memory) + index * 2u;
		*value = sc;
		*(value + 1) = tc;
	}

	GrassTextureAtlasFactory::GrassTextureAtlasFactory(const unsigned int atlas_height, const unsigned int atlas_width, const unsigned int texture_height, const unsigned int texture_width, const unsigned char stride)
		: TextureAtlasFactory(atlas_height, atlas_width, texture_height, texture_width, stride)
	{
	}
	inline void GrassTextureAtlasFactory::textureFiller(void* memory, const unsigned int index, const unsigned int a_h, const unsigned int a_w, const unsigned int t_h, const unsigned int t_w) const
	{
		// parameters
		const float parameter_force = 0.01f;
		const unsigned int parameter_blade_count = 3u;
		struct Grass
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
		const Grass blades[] = {
			{0.33f, 0.55f, 0.045f},
			{0.50f, 0.60f, 0.0475f},
			{0.66f, 0.50f, 0.0425f},
		};

		const float ah = static_cast<float>(static_cast<int>(a_h) - static_cast<int>(atlas_height / 2u)) / static_cast<float>(atlas_height);
		const float aw = static_cast<float>(static_cast<int>(a_w) - static_cast<int>(atlas_width / 2u)) / static_cast<float>(atlas_width);

		const float th = static_cast<float>(t_h) / static_cast<float>(texture_height);
		const float tw = static_cast<float>(t_w) / static_cast<float>(texture_width);

		unsigned char* value = static_cast<unsigned char*>(memory) + index;

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
				*value = 255;
				return;
			}
		}
		*value = 0;
	}

	/*
	extern void* createTextureAtlasMSAA(SubTextureFiller sub_texture_filler, const unsigned char stride, const unsigned int atlas_height, const unsigned int atlas_width, const unsigned int texture_height, const unsigned int texture_width, const unsigned int msaa)
	{
		void* msaa_texture_atlas = createTextureAtlas(sub_texture_filler, stride, atlas_height, atlas_width, texture_height * msaa, texture_width * msaa);

		const unsigned int texture_atlas_height = atlas_height * texture_height;
		const unsigned int texture_atlas_width = atlas_width * texture_width;
		const unsigned int texture_atlas_size = texture_atlas_height * texture_atlas_width;

		// NOTE: using modulus is a lot cleaner but a lot slower
		void* texture_atlas = malloc(texture_atlas_size * stride);
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
						unsigned int total_r = 0u;
						unsigned int total_g = 0u;
						unsigned int total_b = 0u;
						unsigned int total_a = 0u;

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
								total_r += sample.r;
								total_g += sample.g;
								total_b += sample.b;
								total_a += sample.a;
							}
						}

						unsigned int rgba_divisor = msaa * msaa;
						unsigned char avg_r = static_cast<unsigned char>(total_r / rgba_divisor);
						unsigned char avg_g = static_cast<unsigned char>(total_g / rgba_divisor);
						unsigned char avg_b = static_cast<unsigned char>(total_b / rgba_divisor);
						unsigned char avg_a = static_cast<unsigned char>(total_a / rgba_divisor);

						const unsigned int index = base_i_a_h + base_i_t_h + base_i_a_w + t_w;
						texture_atlas[index] = Color(avg_r, avg_g, avg_b, avg_a);
					}
				}
			}
		}
		delete msaa_texture_atlas;
		return texture_atlas;
	}
	*/
}
