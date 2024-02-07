#pragma once

// internal
#include "Vertex.h"
#include "../Builder.h"
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
		Builder(Pipeline::Device& device, Renderable::XZUniform::Builder* ground, Noise::Noise& offsetNoise, const float density) :
			device(device), pointCountSide(density * VERTICES), pointCount(pointCountSide * pointCountSide)
		{
			// alloc memory
			Vertex* vertices = static_cast<Vertex*>(operator new[](sizeof(Vertex) * pointCount));

			for (size_t z = 0; z < pointCountSide; z++)
			{
				const float mapped_z = z / density;
				const size_t min_z = IMath::FloorToInt(mapped_z);
				const size_t max_z = IMath::CeilToInt(mapped_z);
				const float t_z = max_z - mapped_z;

				for (size_t x = 0; x < pointCountSide; x++)
				{
					const float mapped_x = x / density;
					const size_t min_x = IMath::FloorToInt(mapped_x);
					const size_t max_x = IMath::CeilToInt(mapped_x);
					const float t_x = max_x - mapped_x;

					const int mapped_z_i = IMath::Lerp(z_min, z_max, t_z);
				}
			}

			for (size_t i = 0; i < pointCount; i++)
			{
				// calculate xz
				const float x = static_cast<float>(i % density);
				const float z = static_cast<float>(i / density);

				const size_t x_min;
				const size_t x_max;

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
		const size_t pointCountSide;
		Pipeline::Device& device;
		Pipeline::Buffer* vertexBuffer;
	};
}
