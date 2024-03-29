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
		Builder(Pipeline::Device& device) : device(device), pointCount(7 * 7)
		{
			// alloc memory
			Vertex* vertices = static_cast<Vertex*>(operator new[](sizeof(Vertex)* pointCount));

			for (int z = -3; z <= 3; z++)
			{
				for (int x = -3; x <= 3; x++)
				{
					const auto i = (x + 3) + (z + 3) * 7;
					vertices[i].position.x = static_cast<float>(x * 64) * Utilities::PixelPerfectUtility::UNITS_PER_PIXEL;
					vertices[i].position.y = -10.0f * Utilities::PixelPerfectUtility::Y_SCALE;
					vertices[i].position.z = static_cast<float>(z * 64) * Utilities::PixelPerfectUtility::UNITS_PER_PIXEL;
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
			vkCmdDraw(commandBuffer, static_cast<uint32_t>(pointCount), 1, 0, 0);
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

		const uint32_t pointCount;
		Pipeline::Device& device;
		Pipeline::Buffer* vertexBuffer;
	};
}
