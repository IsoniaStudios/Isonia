#pragma once

// internal
#include "../Pipeline/Buffer.h"
#include "../Pipeline/Device.h"
#include "BuilderComplete.h"

// std
#include <memory>
#include <vector>
#include <cassert>
#include <cstring>

namespace Isonia::Renderable
{
	class Model
	{
	public:
		Model(Pipeline::Device& device, const BuilderComplete& builder) : device(device)
		{
			CreateVertexBuffers(builder.vertices);
			CreateIndexBuffers(builder.indices);
		}

		~Model()
		{
		}

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		static Model* CreateModelFromFile(Pipeline::Device& device, const std::string& filepath)
		{
			BuilderComplete builder{};
			builder.LoadModel(filepath);
			return new Model(device, builder);
		}

		void Bind(VkCommandBuffer commandBuffer)
		{
			VkBuffer buffers[] = { vertexBuffer->GetBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

			if (hasIndexBuffer)
			{
				vkCmdBindIndexBuffer(commandBuffer, indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
			}
		}

		void Draw(VkCommandBuffer commandBuffer)
		{
			if (hasIndexBuffer)
			{
				vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
			}
			else
			{
				vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
			}
		}

	private:
		void CreateVertexBuffers(const std::vector<VertexComplete>& vertices)
		{
			vertexCount = static_cast<uint32_t>(vertices.size());
			assert(vertexCount >= 3 && "Vertex count must be at least 3");

			VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
			uint32_t vertexSize = sizeof(vertices[0]);

			Pipeline::Buffer stagingBuffer{
				device,
				vertexSize,
				vertexCount,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			};

			stagingBuffer.Map();
			stagingBuffer.WriteToBuffer((void*)vertices.data());

			vertexBuffer = new Pipeline::Buffer(
				device,
				vertexSize,
				vertexCount,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			device.CopyBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);
		}

		void CreateIndexBuffers(const std::vector<uint32_t>& indices)
		{
			indexCount = static_cast<uint32_t>(indices.size());
			hasIndexBuffer = indexCount > 0;

			if (!hasIndexBuffer)
				return;

			VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
			uint32_t indexSize = sizeof(indices[0]);

			Pipeline::Buffer stagingBuffer{
				device,
				indexSize,
				indexCount,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			};

			stagingBuffer.Map();
			stagingBuffer.WriteToBuffer((void*)indices.data());

			indexBuffer = new Pipeline::Buffer(
				device,
				indexSize,
				indexCount,
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			device.CopyBuffer(stagingBuffer.GetBuffer(), indexBuffer->GetBuffer(), bufferSize);
		}

		Pipeline::Device& device;

		Pipeline::Buffer* vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		Pipeline::Buffer* indexBuffer;
		uint32_t indexCount;
	};
}
