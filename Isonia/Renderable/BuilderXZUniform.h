#pragma once

// internal
#include "VertexXZUniform.h"
#include "../Pipeline/Buffer.h"
#include "../Pipeline/Device.h"

// std
#include <stdexcept>
#include <bitset>

namespace Isonia::Renderable
{
	const std::size_t VERTICES = 19;
	const std::size_t VERTICES_COUNT = VERTICES * VERTICES + (VERTICES - 2) * (VERTICES - 1);

	struct XZPositionalData
	{
		XZPositionalData(const float x, const float z) : x(x), z(z) { }
		const float x;
		const float z;
	};

	struct BuilderXZUniform
	{
		VertexXZUniform vertices[VERTICES_COUNT];
		const XZPositionalData positionalData;

		BuilderXZUniform(Pipeline::Device& device, float x, float z) : device(device), positionalData(x, z)
		{
			for (size_t i = 0; i < VERTICES_COUNT; i++)
			{
				vertices[i].altitude = 0.0;
			}
			CreateVertexBuffers();
		}

		~BuilderXZUniform()
		{
			delete vertexBuffer;
		}

		void Bind(VkCommandBuffer commandBuffer)
		{
			VkBuffer buffers[] = { vertexBuffer->GetBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		}

		void Draw(VkCommandBuffer commandBuffer)
		{
			vkCmdDraw(commandBuffer, VERTICES_COUNT, 1, 0, 0);
		}

	private:
		void CreateVertexBuffers()
		{
			VkDeviceSize bufferSize = sizeof(VertexXZUniform) * VERTICES_COUNT;
			uint32_t vertexSize = sizeof(VertexXZUniform);

			Pipeline::Buffer stagingBuffer{
				device,
				vertexSize,
				VERTICES_COUNT,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			};

			stagingBuffer.Map();
			stagingBuffer.WriteToBuffer((void*)vertices);

			vertexBuffer = new Pipeline::Buffer(
				device,
				vertexSize,
				VERTICES_COUNT,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			device.CopyBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);
		}

		Pipeline::Device& device;
		Pipeline::Buffer* vertexBuffer;
	};
}
