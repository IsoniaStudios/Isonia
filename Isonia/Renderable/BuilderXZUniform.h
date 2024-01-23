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
	const std::size_t STRIP_COUNT = 8;
	const std::size_t VERTICES_PER_STRIP = (STRIP_COUNT + 1) * 2;
	const std::size_t VERTICES = VERTICES_PER_STRIP * STRIP_COUNT;
	const float UNIFORM_DISTANCE = 0.1f;

	struct BuilderXZUniform
	{
		VertexXZUniform vertices[VERTICES];

		BuilderXZUniform(Pipeline::Device& device) : device(device)
		{
			for (size_t i = 0; i < VERTICES; i++)
			{
				vertices[i] = VertexXZUniform{};
			}
			CreateVertexBuffers();
		}

		void Bind(VkCommandBuffer commandBuffer)
		{
			VkBuffer buffers[] = { vertexBuffer->GetBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		}

		void Draw(VkCommandBuffer commandBuffer)
		{
			vkCmdDraw(commandBuffer, VERTICES, 1, 0, 0);
		}

	private:
		void CreateVertexBuffers()
		{
			VkDeviceSize bufferSize = sizeof(VertexXZUniform) * VERTICES;
			uint32_t vertexSize = sizeof(VertexXZUniform);

			Pipeline::Buffer stagingBuffer{
				device,
				vertexSize,
				VERTICES,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			};

			stagingBuffer.Map();
			stagingBuffer.WriteToBuffer((void*)vertices);

			vertexBuffer = new Pipeline::Buffer(
				device,
				vertexSize,
				VERTICES,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			device.CopyBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);
		}

		Pipeline::Device& device;
		Pipeline::Buffer* vertexBuffer;
	};
}
