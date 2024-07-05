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
		return new Model(device, generatePrimitiveVertices(primitive), generatePrimitiveVerticesCount(primitive), generatePrimitiveIndices(primitive), generatePrimitiveIndicesCount(primitive));
	}

	Model* Model::createPrimitiveFace(Pipeline::Device* device, const unsigned int num_sides)
	{
		return new Model(device, generatePrimitiveFaceVertices(num_sides), generatePrimitiveFaceVerticesCount(num_sides), generatePrimitiveFaceIndices(num_sides), generatePrimitiveFaceIndicesCount(num_sides));
	}

	Model* Model::createPrimitivePrism(Pipeline::Device* device, const unsigned int num_sides)
	{
		return new Model(device, generatePrimitivePrismVertices(num_sides), generatePrimitivePrismVerticesCount(num_sides), generatePrimitivePrismIndices(num_sides), generatePrimitivePrismIndicesCount(num_sides));
	}

	Model* Model::createPrimitiveSphere(Pipeline::Device* device, const unsigned int sub_divisions)
	{
		return new Model(device, generatePrimitiveSphereVertices(sub_divisions), generatePrimitiveSphereVerticesCount(sub_divisions), generatePrimitiveSphereIndices(sub_divisions), generatePrimitiveSphereIndicesCount(sub_divisions));
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

		const unsigned int vertex_size = sizeof(VertexComplete);
		const VkDeviceSize buffer_size = sizeof(VertexComplete) * vertex_count;

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

	void Model::createIndexBuffers(const unsigned int* indices, const unsigned int index_count)
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
