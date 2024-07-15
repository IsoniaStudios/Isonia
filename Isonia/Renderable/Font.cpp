// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	Texture Font::createFontMap(Pipeline::Device* device, const unsigned int height, const unsigned int width, const unsigned char* texture_binary) const
	{
		const unsigned int texture_size = width * height;
		unsigned char* texture_byte = (unsigned char*)malloc(texture_size * sizeof(unsigned char));
		for (unsigned int i = 0; i < width; i++)
		{
			const unsigned char binary_pixel = texture_binary[i];
			for (char bit = 0; bit < 8; bit++)
			{
				const unsigned char pixel = (binary_pixel & (1 << bit)) ? 255 : 0;
				const int index = i * 8 + 7 - bit;
				texture_byte[index] = pixel;
			}
		}
		return Texture(device, texture_byte, height, width, VK_FORMAT_R8_UNORM);
	}

	Math::Vector2 Font::sampleFontSingleRowTexture(const char c) const
	{
		const float width = static_cast<float>(m_font_map.getExtent().width);
		return Math::Vector2{
			m_char_to_offsets[c - 32] / width,
			m_char_to_offsets[c - 31] / width
		};
	}

	Font::Font(Pipeline::Device* device, const unsigned int height, const unsigned int width, const unsigned char* texture, const unsigned int char_to_offsets[char_count])
		: m_font_map(createFontMap(device, height, width, texture))
	{
		memcpy(m_char_to_offsets, char_to_offsets, sizeof(m_char_to_offsets));
	}

	const Texture* Font::getTexture() const
	{
		return &m_font_map;
	}
}
