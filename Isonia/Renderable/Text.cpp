// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	extern Texture* create7x7PixelFontSingleRowTexture(Pipeline::Device* device)
	{
		const int pixel_font_single_row_binary_7x7_length = 588;
		const unsigned char pixel_font_single_row_binary_7x7[]
		{
			0xff, 0xdf, 0x5e, 0xbe, 0xf3, 0xf3, 0xe7, 0xf7, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xdf,
			0x1c, 0x1f, 0x70, 0x79, 0xc1, 0xc7, 0x8f, 0xff, 0xff, 0x7f, 0xf7, 0xe3, 0xc7, 0x8e, 0x1e, 0x38,
			0xf0, 0x60, 0xe3, 0xbb, 0x8f, 0x8d, 0xdb, 0xf7, 0x6e, 0xe3, 0x87, 0x8e, 0x1e, 0x38, 0x37, 0x6e,
			0xdd, 0xbb, 0x76, 0x0e, 0x3f, 0xf8, 0xfb, 0xff, 0xcf, 0xfe, 0xff, 0xff, 0xbf, 0xf9, 0xff, 0xbf,
			0xdf, 0xdd, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0x7e, 0xfd, 0xff, 0x80, 0xff, 0xdf, 0x5e, 0xbc, 0x33, 0x6d, 0xf7, 0xef, 0xdf, 0x5f, 0x7f, 0xff,
			0xff, 0xfd, 0xbb, 0x9e, 0xef, 0xbe, 0x77, 0xf7, 0xfd, 0xbb, 0x77, 0x3e, 0x7e, 0xff, 0xfb, 0xdd,
			0xbb, 0x76, 0xed, 0xdb, 0x77, 0xef, 0xdd, 0xbb, 0xdf, 0xdd, 0xbb, 0xf2, 0x66, 0xdd, 0xbb, 0x76,
			0xed, 0xde, 0xf7, 0x6e, 0xdd, 0xbb, 0x77, 0xee, 0xfb, 0xfe, 0xf5, 0xff, 0xef, 0xfe, 0xff, 0xff,
			0xbf, 0xf7, 0xff, 0xbf, 0xff, 0xfd, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xff, 0x80, 0xff, 0xdf, 0x5c, 0x1a, 0xfe, 0xeb, 0xef, 0xdf,
			0xef, 0xbf, 0x7f, 0xff, 0xff, 0xfb, 0xb3, 0xdf, 0xef, 0x7d, 0x70, 0xef, 0xfb, 0xbb, 0x77, 0x3e,
			0x7d, 0xf0, 0x7d, 0xfd, 0xfb, 0x76, 0xed, 0xfb, 0xb7, 0xef, 0xdf, 0xbb, 0xdf, 0xdd, 0x7b, 0xf5,
			0x66, 0xdd, 0xbb, 0x76, 0xed, 0xfe, 0xf7, 0x6e, 0xdd, 0xd7, 0x77, 0xde, 0xfd, 0xfe, 0xee, 0xff,
			0xf7, 0x8e, 0x9e, 0x3c, 0xb8, 0xe3, 0xe1, 0xa7, 0x9f, 0x9d, 0xbe, 0xf2, 0xe9, 0xe3, 0x87, 0x86,
			0x9e, 0x1d, 0xf7, 0x6e, 0xdd, 0x9b, 0x76, 0x0e, 0xfe, 0xfe, 0xf6, 0x80, 0xff, 0xdf, 0xfe, 0xbc,
			0x7d, 0xf7, 0xff, 0xdf, 0xef, 0x5c, 0x1f, 0xf0, 0x7f, 0xf7, 0xab, 0xdf, 0xdf, 0xbb, 0x7f, 0x61,
			0xf7, 0xc7, 0x87, 0xff, 0xfb, 0xff, 0xfe, 0xfb, 0xcb, 0x06, 0x1d, 0xfb, 0xb0, 0xe1, 0xd1, 0x83,
			0xdf, 0xdc, 0xfb, 0xf5, 0x6a, 0xdd, 0x87, 0x76, 0x1e, 0x3e, 0xf7, 0x6e, 0xd5, 0xef, 0xaf, 0xbe,
			0xfe, 0xfe, 0xff, 0xff, 0xff, 0xf6, 0x6d, 0xfb, 0x37, 0x77, 0xdd, 0x9b, 0xdf, 0xdd, 0x7e, 0xf5,
			0x66, 0xdd, 0xbb, 0x76, 0x6d, 0xfd, 0xf7, 0x6e, 0xdd, 0xe7, 0xb7, 0xdd, 0xfe, 0xff, 0x6a, 0x80,
			0xff, 0xdf, 0xfc, 0x1e, 0xbb, 0xea, 0xff, 0xdf, 0xef, 0xff, 0x7c, 0xff, 0xff, 0xef, 0x9b, 0xdf,
			0xbf, 0xd8, 0x3f, 0x6e, 0xf7, 0xbb, 0xf7, 0x3e, 0x7d, 0xf0, 0x7d, 0xf7, 0xab, 0x76, 0xed, 0xfb,
			0xb7, 0xef, 0xdd, 0xbb, 0xdf, 0xdd, 0x7b, 0xf7, 0x6c, 0xdd, 0xbf, 0x56, 0xbf, 0xde, 0xf7, 0x6e,
			0xd5, 0xd7, 0xdf, 0x7e, 0xff, 0x7e, 0xff, 0xff, 0xff, 0x86, 0xed, 0xfb, 0xb0, 0x77, 0xe1, 0xbb,
			0xdf, 0xdc, 0xfe, 0xf5, 0x6e, 0xdd, 0x87, 0x86, 0xfe, 0x3d, 0xf7, 0x75, 0xd5, 0xef, 0xcf, 0xbe,
			0xfe, 0xfe, 0xed, 0x80, 0xff, 0xff, 0xfe, 0xb8, 0x76, 0x6d, 0xff, 0xef, 0xdf, 0xff, 0x7e, 0xff,
			0xf3, 0xdf, 0xbb, 0xdf, 0x7d, 0xdf, 0x77, 0x6e, 0xf7, 0xbb, 0xef, 0x3f, 0x7e, 0xff, 0xfb, 0xff,
			0xab, 0x76, 0xed, 0xdb, 0x77, 0xef, 0xdd, 0xbb, 0xde, 0xdd, 0xbb, 0xf7, 0x6e, 0xdd, 0xbf, 0x6e,
			0xdd, 0xde, 0xf7, 0x75, 0xd5, 0xbb, 0xde, 0xfe, 0xff, 0xbe, 0xff, 0xff, 0xff, 0x76, 0xed, 0xdb,
			0xb7, 0xf7, 0xfd, 0xbb, 0xde, 0xdd, 0x7e, 0xf5, 0x6e, 0xdd, 0xbf, 0xf6, 0xff, 0xdd, 0xb6, 0x75,
			0xd5, 0xcf, 0xdf, 0x7e, 0xfe, 0xfe, 0xff, 0x80, 0xff, 0xdf, 0xfe, 0xbe, 0xfe, 0x72, 0xff, 0xf7,
			0xbf, 0xff, 0xfd, 0xff, 0xf3, 0xff, 0xc7, 0x8e, 0x0e, 0x3f, 0x78, 0xf1, 0xf7, 0xc7, 0x9f, 0xfe,
			0xff, 0x7f, 0xf7, 0xf7, 0xc7, 0x76, 0x1e, 0x38, 0xf0, 0x6f, 0xe1, 0xbb, 0x8f, 0x3d, 0xd8, 0x37,
			0x6e, 0xe3, 0xbf, 0x96, 0xee, 0x3e, 0xf8, 0xfb, 0xeb, 0xbb, 0xde, 0x0e, 0x3f, 0xf8, 0xff, 0xc1,
			0xff, 0x86, 0x1e, 0x3c, 0x38, 0xf7, 0xe3, 0xbb, 0x8f, 0x3d, 0xbc, 0x75, 0x6e, 0xe3, 0xbf, 0xf6,
			0xfc, 0x3e, 0x79, 0x7b, 0xeb, 0xb3, 0x3e, 0x0f, 0x7e, 0xfd, 0xff, 0x80
		};
		unsigned char pixel_font_single_row_byte_7x7[pixel_font_single_row_binary_7x7_length * 8];
		for (int i = 0; i < pixel_font_single_row_binary_7x7_length; i++)
		{
			const unsigned char binary_pixel = pixel_font_single_row_binary_7x7[i];
			//for (char bit = 7; bit >= 0; bit--) // big endian
			for (char bit = 0; bit < 8; bit++) // big endian
			{
				const unsigned char pixel = (binary_pixel & (1 << bit)) ? 255 : 0;
				const int index = i * 8 + 7 - bit;
				pixel_font_single_row_byte_7x7[index] = pixel;
			}
		}
		return Renderable::Texture::createTexture(device, pixel_font_single_row_byte_7x7, 672u, 7u, VK_FORMAT_R8_UNORM);
	}

	BuilderUI::BuilderUI(Pipeline::Device* device, const char* text)
		: m_device(device)
	{
		const unsigned int char_length = getCharLength(text);

		const unsigned int vertices_per_quad = 4u;
		const unsigned int indices_per_quad = 6u;

		const unsigned int vertices_count = vertices_per_quad * char_length;
		const unsigned int indices_count = indices_per_quad * char_length;

		float x = -0.5f;
		float y = 0.0f;

		VertexUI* vertices = new VertexUI[vertices_count];
		unsigned int* indices = new unsigned int[indices_count];

		const float test = 0.05f;

		for (unsigned int i = 0; i < char_length; i++)
		{
			const unsigned char c = text[i];

			const float p_x = charToSingleRowMonoASCIIOffset(c);
			const float p_z = charToSingleRowMonoASCIIOffset(c);
			const float uv_x = charToSingleRowMonoASCIIOffset(c);
			VertexUI p0{ x, y, uv_x, 0.0f };
			VertexUI p1{ x + test, y, uv_x + charToSingleRowMonoASCIIWidth(), 0.0f };
			VertexUI p2{ x, y + test, uv_x, 1.0f };
			VertexUI p3{ x + test, y + test, uv_x + charToSingleRowMonoASCIIWidth(), 1.0f };

			vertices[(i * vertices_per_quad) + 0] = p0;
			vertices[(i * vertices_per_quad) + 1] = p1;
			vertices[(i * vertices_per_quad) + 2] = p2;
			vertices[(i * vertices_per_quad) + 3] = p3;

			indices[(i * indices_per_quad) + 0] = (i * vertices_per_quad) + 0;
			indices[(i * indices_per_quad) + 1] = (i * vertices_per_quad) + 2;
			indices[(i * indices_per_quad) + 2] = (i * vertices_per_quad) + 1;
			indices[(i * indices_per_quad) + 3] = (i * vertices_per_quad) + 1;
			indices[(i * indices_per_quad) + 4] = (i * vertices_per_quad) + 2;
			indices[(i * indices_per_quad) + 5] = (i * vertices_per_quad) + 3;

			x += test;

			if (c == '\n')
			{
				x = -0.5f;
				y += test;
			}
			else if (c == '\t')
			{
				x += test * 4.0f;
			}
		}

		createVertexBuffers(vertices, vertices_count);
		createIndexBuffers(indices, indices_count);
	}

	BuilderUI::~BuilderUI()
	{
		delete m_vertex_buffer;
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

	void BuilderUI::createVertexBuffers(const VertexUI* vertices, const unsigned int vertex_count)
	{
		m_vertex_count = vertex_count;

		const unsigned int vertex_size = sizeof(VertexUI);
		const VkDeviceSize buffer_size = sizeof(VertexUI) * vertex_count;

		Pipeline::Buffer staging_buffer{
			m_device,
			vertex_size,
			vertex_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		staging_buffer.map();
		staging_buffer.writeToBuffer((void*)vertices);

		m_vertex_buffer = new Pipeline::Buffer(
			m_device,
			vertex_size,
			vertex_count,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_device->copyBuffer(staging_buffer.getBuffer(), m_vertex_buffer->getBuffer(), buffer_size);
	}

	void BuilderUI::createIndexBuffers(const unsigned int* indices, const unsigned int index_count)
	{
		m_index_count = index_count;

		const unsigned int index_size = sizeof(unsigned int);
		const VkDeviceSize buffer_size = sizeof(unsigned int) * index_count;

		Pipeline::Buffer staging_buffer{
			m_device,
			index_size,
			index_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		staging_buffer.map();
		staging_buffer.writeToBuffer((void*)indices);

		m_index_buffer = new Pipeline::Buffer(
			m_device,
			index_size,
			index_count,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_device->copyBuffer(staging_buffer.getBuffer(), m_index_buffer->getBuffer(), buffer_size);
	}
}
