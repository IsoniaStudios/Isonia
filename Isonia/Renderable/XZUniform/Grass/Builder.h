#pragma once

// internal
#include "Vertex.h"
#include "../../../Noise/Noise.h"
#include "../../../Pipeline/Buffer.h"
#include "../../../Pipeline/Device.h"

// external
#include <glm/glm.hpp>

// std
#include <stdexcept>
#include <bitset>
#include <cmath>

namespace Isonia::Renderable::XZUniform::Grass
{
	struct Builder
	{
		Builder(Pipeline::Device& device, Renderable::XZUniform::Builder* ground, Noise::Noise& placementNoise, Noise::Noise& offsetNoise) : device(device)
		{
			// alloc memory
			Vertex* vertices = static_cast<Vertex*>(operator new[](sizeof(Vertex) * UNIQUE_VERTICES_COUNT));

			for (size_t i = 0; i < UNIQUE_VERTICES_COUNT; i++)
			{
				// calculate xz
				const size_t x = i % VERTICES;
				const size_t z = i / VERTICES;

				vertices[i].position = glm::vec3(
					static_cast<float>(x) * QUAD_SIZE + ground->positionalData.x,
					ground->sampleAltitudes[(x + 1) + SAMPLE * (z + 1)],
					static_cast<float>(z) * QUAD_SIZE + ground->positionalData.z
				);

				auto normal = ground->normals[x + VERTICES * z];
				vertices[i].pitch = glm::atan(normal.y, normal.z);
				vertices[i].yaw = glm::atan(normal.y, normal.x);

				vertices[i].gain = 0.0f;
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
			vkCmdDraw(commandBuffer, UNIQUE_VERTICES_COUNT, 1, 0, 0);
		}

	private:
		void CreateVertexBuffers(void* vertices)
		{
			const VkDeviceSize bufferSize = sizeof(Vertex) * UNIQUE_VERTICES_COUNT;
			const uint32_t vertexSize = sizeof(Vertex);

			Pipeline::Buffer stagingBuffer{
				device,
				vertexSize,
				UNIQUE_VERTICES_COUNT,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			};

			stagingBuffer.Map();
			stagingBuffer.WriteToBuffer(vertices);

			vertexBuffer = new Pipeline::Buffer(
				device,
				vertexSize,
				UNIQUE_VERTICES_COUNT,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			device.CopyBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);
		}

		Pipeline::Device& device;
		Pipeline::Buffer* vertexBuffer;
	};
}
