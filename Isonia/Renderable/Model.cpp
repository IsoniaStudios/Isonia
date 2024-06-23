// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	Model::Model(Pipeline::Device* device, const VertexComplete* vertices, const unsigned int vertices_count, const unsigned int* indices, const unsigned int indices_count)
		: m_device(device)
	{
		createVertexBuffers(vertices, vertices_count);
		createIndexBuffers(indices, indices_count);
	}

	Model::~Model()
	{
		delete m_vertex_buffer;
		delete m_index_buffer;
	}

	Model* Model::createPrimitive(Pipeline::Device* device, const PrimitiveType primitive)
	{
		const std::vector<VertexComplete> vertices = generatePrimitiveVertices(primitive);
		const std::vector<unsigned int> indices = generatePrimitiveIndices(primitive);
		return new Model(device, &vertices, &indices);
	}

	Model* Model::createPrimitiveFace(Pipeline::Device* device, const unsigned int num_sides)
	{
		const std::vector<VertexComplete> vertices = generatePrimitiveFaceVertices(num_sides);
		const std::vector<unsigned int> indices = generatePrimitiveFaceIndices(num_sides);
		return new Model(device, &vertices, &indices);
	}

	Model* Model::createPrimitivePrism(Pipeline::Device* device, const unsigned int num_sides)
	{
		const std::vector<VertexComplete> vertices = generatePrimitivePrismVertices(num_sides);
		const std::vector<unsigned int> indices = generatePrimitivePrismIndices(num_sides);
		return new Model(device, &vertices, &indices);
	}

	Model* Model::createPrimitiveSphere(Pipeline::Device* device, const unsigned int sub_divisions)
	{
		const std::vector<VertexComplete> vertices = generatePrimitiveSphereVertices(sub_divisions);
		const std::vector<unsigned int> indices = generatePrimitiveSphereIndices(sub_divisions);
		return new Model(device, &vertices, &indices);
	}

	void Model::bind(VkCommandBuffer command_buffer)
	{
		VkBuffer buffers[] = { m_vertex_buffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);
		vkCmdBindIndexBuffer(command_buffer, m_index_buffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}

	void Model::draw(VkCommandBuffer command_buffer)
	{
		vkCmdDrawIndexed(command_buffer, m_index_count, 1, 0, 0, 0);
	}

	void Model::createVertexBuffers(const VertexComplete* vertices, const unsigned int vertex_count)
	{
		m_vertex_count = vertex_count;
		assert(vertex_count >= 3 && "Vertex count must be at least 3");

		const unsigned int vertexSize = sizeof(VertexComplete);
		const VkDeviceSize bufferSize = sizeof(VertexComplete) * vertex_count;

		Pipeline::Buffer stagingBuffer{
			m_device,
			vertexSize,
			vertex_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices);

		m_vertex_buffer = new Pipeline::Buffer(
			m_device,
			vertexSize,
			vertex_count,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_device->copyBuffer(stagingBuffer.getBuffer(), m_vertex_buffer->getBuffer(), bufferSize);
	}

	void Model::createIndexBuffers(const unsigned int* indices, const unsigned int indexCount)
	{
		m_index_count = indexCount;

		const unsigned int indexSize = sizeof(unsigned int);
		const VkDeviceSize bufferSize = sizeof(unsigned int) * indexCount;

		Pipeline::Buffer stagingBuffer{
			m_device,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices);

		m_index_buffer = new Pipeline::Buffer(
			m_device,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_device->copyBuffer(stagingBuffer.getBuffer(), m_index_buffer->getBuffer(), bufferSize);
	}
}
