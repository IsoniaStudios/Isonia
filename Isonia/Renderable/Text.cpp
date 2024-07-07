// internal
#include "Renderable.h"

// external
#include <stdexcept>

namespace Isonia::Renderable
{
	extern Texture* create3x6PixelFontSingleRowTexture(Pipeline::Device* device)
	{
		const int pixel_font_single_row_binary_3x6_length = 376;
		const unsigned char pixel_font_single_row_binary_3x6[pixel_font_single_row_binary_3x6_length]
		{
			0x0a, 0x94, 0x40, 0x42, 0x52, 0x80, 0x00, 0x99, 0x33, 0x2b, 0x9b, 0x9b, 0x00, 0x00, 0x31, 0xc6,
			0xc6, 0xce, 0x66, 0xae, 0xea, 0x8f, 0x31, 0xb9, 0xb9, 0xba, 0xaa, 0x2a, 0xae, 0xd1, 0x80, 0x20,
			0x80, 0x20, 0x20, 0x89, 0x44, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0xa0, 0x00, 0x0a,
			0xbe, 0x68, 0xa2, 0x89, 0x00, 0x00, 0xab, 0x08, 0xaa, 0x20, 0xaa, 0x80, 0x82, 0x0a, 0x6a, 0xa8,
			0xa8, 0x88, 0xa4, 0x2a, 0x8a, 0xaa, 0xaa, 0xaa, 0x12, 0xaa, 0xaa, 0xa2, 0x90, 0x90, 0x2c, 0xc6,
			0x66, 0x46, 0xc0, 0x44, 0xd3, 0x1b, 0x99, 0x99, 0x2a, 0xa2, 0xaa, 0xe4, 0x90, 0x00, 0x08, 0x14,
			0x82, 0x40, 0x8a, 0x90, 0x01, 0x29, 0x0b, 0x1b, 0x30, 0x92, 0xa9, 0x39, 0x0a, 0xae, 0xe8, 0xac,
			0xc8, 0xe4, 0x2c, 0x8a, 0xaa, 0xb2, 0xb3, 0x92, 0xaa, 0xa4, 0xa4, 0x88, 0xa8, 0x02, 0xa8, 0xaa,
			0x4a, 0xa9, 0x54, 0xaa, 0xaa, 0xaa, 0x23, 0xaa, 0xaa, 0xaa, 0x24, 0x91, 0x80, 0x08, 0x14, 0xe4,
			0xa8, 0x88, 0x38, 0xe1, 0x29, 0x10, 0x88, 0xa9, 0x29, 0x82, 0x00, 0x92, 0xea, 0xa8, 0xa8, 0x8a,
			0xa4, 0x2a, 0x8a, 0xaa, 0xa2, 0xa8, 0x92, 0xaa, 0xa4, 0x44, 0x88, 0x80, 0x06, 0xa8, 0xae, 0xea,
			0xa9, 0x64, 0xaa, 0xaa, 0xaa, 0x39, 0x2a, 0xaa, 0x4a, 0x48, 0x8b, 0x00, 0x00, 0x3e, 0x28, 0x90,
			0x88, 0x10, 0x02, 0x29, 0x20, 0x88, 0xa9, 0x28, 0x81, 0x39, 0x02, 0x0a, 0xa8, 0xa8, 0x8a, 0xa4,
			0x2a, 0x8a, 0xaa, 0xa2, 0xa8, 0x92, 0xaa, 0xaa, 0x48, 0x84, 0x80, 0x0a, 0xa8, 0xa8, 0x4a, 0xa9,
			0x54, 0xaa, 0xaa, 0xaa, 0x09, 0x2a, 0xaa, 0xaa, 0x84, 0x90, 0x00, 0x08, 0x14, 0xc2, 0x68, 0x88,
			0x02, 0x0a, 0x33, 0xbb, 0x0b, 0x19, 0x33, 0x28, 0x82, 0x11, 0xea, 0xee, 0xce, 0x8e, 0xae, 0xca,
			0xe8, 0xab, 0x23, 0x2b, 0x13, 0x91, 0x4a, 0x4e, 0x84, 0x83, 0x8e, 0xee, 0xe6, 0x4e, 0xa9, 0x52,
			0xaa, 0xb3, 0x3a, 0x31, 0x39, 0x14, 0xa6, 0xe4, 0x90, 0x00, 0x00, 0x00, 0x40, 0x00, 0x50, 0x02,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0xc1, 0x80, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00,
			0x02, 0x08, 0x00, 0x00, 0x00, 0x02, 0x02, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x02, 0x00, 0x00, 0x02,
			0x08, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00
		};
		unsigned char pixel_font_single_row_byte_3x6[pixel_font_single_row_binary_3x6_length * 8];
		for (int i = 0; i < pixel_font_single_row_binary_3x6_length; i++)
		{
			const unsigned char binary_pixel = pixel_font_single_row_binary_3x6[i];
			for (char bit = 0; bit < 8; bit++)
			{
				const unsigned char pixel = (binary_pixel & (1 << bit)) ? 255 : 0;
				const int index = i * 8 + 7 - bit;
				pixel_font_single_row_byte_3x6[index] = pixel;
			}
		}
		return Renderable::Texture::createTexture(device, pixel_font_single_row_byte_3x6, pixel_font_single_row_binary_3x6_length, 8u, VK_FORMAT_R8_UNORM);
	}
	
	extern Math::Vector2 sample3x6PixelFontSingleRowTexture(const char c)
	{
		const int pixel_font_single_row_binary_3x6[]
		{
			  0,   3,   5,   9,  15,  19,  23,  29,  31,  34,  37,  41,
			 45,  47,  51,  53,  57,  61,  65,  69,  73,  77,  81,  85,
			 89,  93,  97,  99, 101, 105, 109, 113, 117, 123, 127, 131,
			135, 139, 143, 147, 151, 155, 159, 163, 167, 171, 177, 181,
			185, 189, 193, 197, 201, 205, 209, 213, 219, 223, 227, 231,
			234, 238, 241, 245, 249, 251, 255, 259, 263, 267, 271, 275,
			279, 283, 285, 288, 292, 295, 301, 305, 309, 313, 317, 321,
			325, 329, 333, 337, 343, 347, 351, 355, 359, 361, 365, 369
		};
		return Math::Vector2 {
			pixel_font_single_row_binary_3x6[c - 32] / 376.0f,
			pixel_font_single_row_binary_3x6[c - 31] / 376.0f
		};
	}

	BuilderUI::BuilderUI(Pipeline::Device* device, const unsigned int max_text_length)
		: m_device(device), m_max_text_length(max_text_length),
		  m_vertex_count(vertices_per_quad * max_text_length), m_index_count(indices_per_quad * max_text_length),
		  m_vertices(new VertexUI[m_vertex_count]{}), m_indices(new unsigned int[m_index_count]{})
	{
		const unsigned int vertex_size = sizeof(VertexUI);
		const unsigned int index_size = sizeof(unsigned int);

		m_vertex_staging_buffer = new Pipeline::Buffer(
			m_device,
			vertex_size,
			m_vertex_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);

		m_index_staging_buffer = new Pipeline::Buffer(
			m_device,
			index_size,
			m_index_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);

		m_vertex_buffer = new Pipeline::Buffer(
			m_device,
			vertex_size,
			m_vertex_count,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_index_buffer = new Pipeline::Buffer(
			m_device,
			index_size,
			m_index_count,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
	}

	void BuilderUI::update(const VkExtent2D extent, const char* text)
	{
		const unsigned int char_length = getCharLength(text);
		assert(char_length < m_max_text_length && "Tried to write outside of buffer!");

		float x = -0.95f;
		float y = -0.95f;

		const float pixel_screen_size_x = 2.0 / extent.width;
		const float pixel_screen_size_y = 2.0 / extent.height;

		for (unsigned int i = 0; i < char_length; i++)
		{
			const unsigned char c = text[i];

			const Math::Vector2 uv_x = sample3x6PixelFontSingleRowTexture(c);
			const float offset_screen_x = pixel_screen_size_x * (uv_x.y - uv_x.x) * 376.0f;
			const float offset_screen_y = pixel_screen_size_y * 8.0f;

			if (c == '\n')
			{
				x = -0.95f;
				y += offset_screen_y;
				continue;
			}
			else if (c == '\t')
			{
				x += offset_screen_x * 4.0f;
				continue;
			}

			VertexUI p0{ x, y, uv_x.x, 0.0f };
			VertexUI p1{ x + offset_screen_x, y, uv_x.y, 0.0f };
			VertexUI p2{ x, y + offset_screen_y, uv_x.x, 1.0f };
			VertexUI p3{ x + offset_screen_x, y + offset_screen_y, uv_x.y, 1.0f };

			m_vertices[(i * vertices_per_quad) + 0] = p0;
			m_vertices[(i * vertices_per_quad) + 1] = p1;
			m_vertices[(i * vertices_per_quad) + 2] = p2;
			m_vertices[(i * vertices_per_quad) + 3] = p3;

			m_indices[(i * indices_per_quad) + 0] = (i * vertices_per_quad) + 0;
			m_indices[(i * indices_per_quad) + 1] = (i * vertices_per_quad) + 2;
			m_indices[(i * indices_per_quad) + 2] = (i * vertices_per_quad) + 1;
			m_indices[(i * indices_per_quad) + 3] = (i * vertices_per_quad) + 1;
			m_indices[(i * indices_per_quad) + 4] = (i * vertices_per_quad) + 2;
			m_indices[(i * indices_per_quad) + 5] = (i * vertices_per_quad) + 3;

			x += offset_screen_x;
		}
		memset(&m_indices[char_length * indices_per_quad], 0, (m_max_text_length - char_length) * sizeof(int));

		m_vertex_staging_buffer->map();
		m_vertex_staging_buffer->writeToBuffer(m_vertices);
		m_device->copyBuffer(m_vertex_staging_buffer->getBuffer(), m_vertex_buffer->getBuffer(), sizeof(VertexUI) * m_vertex_count);
		m_vertex_staging_buffer->unmap();

		m_index_staging_buffer->map();
		m_index_staging_buffer->writeToBuffer(m_indices);
		m_device->copyBuffer(m_index_staging_buffer->getBuffer(), m_index_buffer->getBuffer(), sizeof(unsigned int) * m_index_count);
		m_index_staging_buffer->unmap();
	}

	BuilderUI::~BuilderUI()
	{
		delete m_vertices;
		delete m_vertex_staging_buffer;
		delete m_vertex_buffer;
		delete m_indices;
		delete m_index_staging_buffer;
		delete m_index_buffer;
	}

	float BuilderUI::charToSingleRowMonoASCIIOffset(const char character)
	{
		return (character - 32) * charToSingleRowMonoASCIIWidth();
	}
	float BuilderUI::charToSingleRowMonoASCIIWidth()
	{
		return 1.0f / 96.0f;
	}

	unsigned int BuilderUI::getCharLength(const char* text)
	{
		unsigned int length = 0;
		while (text[length] != '\0')
		{
			length++;
		}
		return length;
	}

	void BuilderUI::bind(VkCommandBuffer command_buffer)
	{
		VkBuffer buffers[] = { m_vertex_buffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);
		vkCmdBindIndexBuffer(command_buffer, m_index_buffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}

	void BuilderUI::draw(VkCommandBuffer command_buffer)
	{
		vkCmdDrawIndexed(command_buffer, m_index_count, 1, 0, 0, 0);
	}
}
