// internal
#include "Renderable.h"

// external
#include <stdexcept>

namespace Isonia::Renderable
{
	BuilderUI::BuilderUI(Pipeline::Device* device, const Font* font, const unsigned int max_text_length)
		: m_device(device), m_font(font), m_max_text_length(max_text_length), m_vertex_count(vertices_per_quad* max_text_length), m_vertices(new VertexUI[m_vertex_count]{}), m_index_count(indices_per_quad* max_text_length)
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

		m_vertex_buffer = new Pipeline::Buffer(
			m_device,
			vertex_size,
			m_vertex_count,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		unsigned int* indices = new unsigned int[m_index_count];
		for (unsigned int i = 0; i < max_text_length; i++)
		{
			indices[(i * indices_per_quad) + 0] = (i * vertices_per_quad) + 0;
			indices[(i * indices_per_quad) + 1] = (i * vertices_per_quad) + 2;
			indices[(i * indices_per_quad) + 2] = (i * vertices_per_quad) + 1;
			indices[(i * indices_per_quad) + 3] = (i * vertices_per_quad) + 1;
			indices[(i * indices_per_quad) + 4] = (i * vertices_per_quad) + 2;
			indices[(i * indices_per_quad) + 5] = (i * vertices_per_quad) + 3;
		}
		Pipeline::Buffer index_staging_buffer = Pipeline::Buffer{
			m_device,
			index_size,
			m_index_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		m_index_buffer = new Pipeline::Buffer(
			m_device,
			index_size,
			m_index_count,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		index_staging_buffer.map();
		index_staging_buffer.writeToBuffer(indices);
		m_device->copyBuffer(index_staging_buffer.getBuffer(), m_index_buffer->getBuffer(), sizeof(unsigned int) * m_index_count);
		delete[] indices;
	}

	void BuilderUI::update(const VkExtent2D extent, const char* text)
	{
		const unsigned int char_length = getCharLength(text);
		assert(char_length < m_max_text_length && "Tried to write outside of buffer!");

		float x = -0.95f;
		float y = -0.95f;

		const VkExtent3D font_extend = m_font->getTexture()->getExtent();

		const float pixel_screen_size_x = 2.0f / static_cast<float>(extent.width);
		const float pixel_screen_size_y = 2.0f / static_cast<float>(extent.height);
		const float offset_screen_y = pixel_screen_size_y * static_cast<float>(font_extend.height);

		for (unsigned int i = 0; i < char_length; i++)
		{
			const unsigned char c = text[i];

			if (c == '\n')
			{
				x = -0.95f;
				y += offset_screen_y;
				memset(&m_vertices[i * vertices_per_quad], 0, vertices_per_quad * sizeof(VertexUI));
				continue;
			}
			else if (c == '\t')
			{
				const Math::Vector2 uv_x = m_font->sampleFontSingleRowTexture(' ');
				x += pixel_screen_size_x * (uv_x.y - uv_x.x) * static_cast<float>(font_extend.width);
				memset(&m_vertices[i * vertices_per_quad], 0, vertices_per_quad * sizeof(VertexUI));
				continue;
			}

			const Math::Vector2 uv_x = m_font->sampleFontSingleRowTexture(c);
			const float offset_screen_x = pixel_screen_size_x * (uv_x.y - uv_x.x) * static_cast<float>(font_extend.width);

			VertexUI p0{ x, y, uv_x.x, 0.0f };
			VertexUI p1{ x + offset_screen_x, y, uv_x.y, 0.0f };
			VertexUI p2{ x, y + offset_screen_y, uv_x.x, 1.0f };
			VertexUI p3{ x + offset_screen_x, y + offset_screen_y, uv_x.y, 1.0f };

			m_vertices[(i * vertices_per_quad) + 0] = p0;
			m_vertices[(i * vertices_per_quad) + 1] = p1;
			m_vertices[(i * vertices_per_quad) + 2] = p2;
			m_vertices[(i * vertices_per_quad) + 3] = p3;

			x += offset_screen_x;
		}

		const unsigned int write_length = Math::maxi(m_previous_char_length, char_length);
		memset(&m_vertices[char_length * vertices_per_quad], 0, (write_length - char_length) * vertices_per_quad * sizeof(VertexUI));
		m_vertex_staging_buffer->map();
		m_vertex_staging_buffer->writeToBuffer(m_vertices);
		m_device->copyBuffer(m_vertex_staging_buffer->getBuffer(), m_vertex_buffer->getBuffer(), sizeof(VertexUI) * vertices_per_quad * write_length);
		m_vertex_staging_buffer->unmap();
		m_previous_char_length = char_length;
	}

	BuilderUI::~BuilderUI()
	{
		delete m_vertices;
		delete m_vertex_staging_buffer;
		delete m_vertex_buffer;
		delete m_index_buffer;
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
