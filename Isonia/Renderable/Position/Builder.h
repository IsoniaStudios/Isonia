#pragma once

// internal
#include "Vertex.h"

#include "../../Pipeline/Buffer.h"
#include "../../Pipeline/Device.h"

#include "../../Utilities/PixelPerfectUtility.h"

// external
#include <glm/glm.hpp>

// std
#include <stdexcept>
#include <bitset>
#include <cmath>

namespace Isonia::Renderable::Position
{
	struct Builder
	{
		Builder(Pipeline::Device& device) : device(device), pointCount(3 * 3)
		{
			// alloc memory
			Vertex* vertices = static_cast<Vertex*>(operator new[](sizeof(Vertex)* pointCount));

			for (size_t x = 0; x < 3; x++)
			{
				for (size_t y = 0; y < 3; y++)
				{
					const auto i = x * 3 + y;
					vertices[i].position.x = static_cast<float>(x * 512) * Utilities::PixelPerfectUtility::UNITS_PER_PIXEL;
					vertices[i].position.y = -static_cast<float>(y * 512) * Utilities::PixelPerfectUtility::UNITS_PER_PIXEL * Utilities::PixelPerfectUtility::Y_SCALE;
					vertices[i].position.z = 0;
				}
			}

			// create the buffers
			CreateVertexBuffers(vertices);

			// free
			delete vertices;
		}

		~Builder()
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
			vkCmdDraw(commandBuffer, pointCount, 1, 0, 0);
		}

	private:
		void CreateVertexBuffers(void* vertices)
		{
			const uint32_t vertexCount = static_cast<uint32_t>(pointCount);

			const VkDeviceSize bufferSize = sizeof(Vertex) * pointCount;
			const uint32_t vertexSize = sizeof(Vertex);

			Pipeline::Buffer stagingBuffer{
				device,
				vertexSize,
				vertexCount,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			};

			stagingBuffer.Map();
			stagingBuffer.WriteToBuffer(vertices);

			vertexBuffer = new Pipeline::Buffer(
				device,
				vertexSize,
				vertexCount,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			device.CopyBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);
		}

		const size_t pointCount;
		Pipeline::Device& device;
		Pipeline::Buffer* vertexBuffer;
	};
}
