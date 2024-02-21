#pragma once

// internal
#include "Vertex.h"
#include "../Builder.h"
#include "../../../Noise/WhiteNoise.h"
#include "../../../Pipeline/Buffer.h"
#include "../../../Pipeline/Device.h"
#include "../../../Utilities/PixelPerfectUtility.h"

// external
#include <glm/glm.hpp>

// std
#include <stdexcept>
#include <bitset>
#include <cmath>

namespace Isonia::Renderable::XZUniformN::Grass
{
	static constexpr const float GRASS_DENSITY = 4.0f;
	static constexpr const float GRASS_SIZE = Utilities::PixelPerfectUtility::PIXELS_PER_UNIT / (16.0f * 2.0f);
	static constexpr const uint32_t GRASS_COUNT_SIDE = static_cast<uint32_t>(GRASS_DENSITY * static_cast<float>(QUADS));
	static constexpr const uint32_t GRASS_COUNT = GRASS_COUNT_SIDE * GRASS_COUNT_SIDE;

	struct Builder
	{
		Builder(Pipeline::Device& device, Renderable::XZUniformN::Builder* ground) :
				device(device)
		{
			// alloc memory
			Vertex* vertices = static_cast<Vertex*>(operator new[](sizeof(Vertex) * GRASS_COUNT));
			
			const Noise::WhiteNoise offsetNoise = {};
			const float size = QUAD_SIZE / GRASS_DENSITY;
			const float half_size = size * 0.5f;

			for (uint32_t z = 0; z < GRASS_COUNT_SIDE; z++)
			{
				for (uint32_t x = 0; x < GRASS_COUNT_SIDE; x++)
				{
					float world_z = z * size + ground->positionalData.z;
					float world_x = x * size + ground->positionalData.x;
					world_x += offsetNoise.GenerateNoise(world_x, world_z, 0.0f) * half_size;
					world_z += offsetNoise.GenerateNoise(world_x, world_z, 1.0f) * half_size;

					float world_y = ground->MapWorldToHeight(world_x, world_z);
					world_y -= GRASS_SIZE * Utilities::PixelPerfectUtility::Y_SCALE;

					const glm::vec3 world_normal = ground->MapWorldToNormal(world_x, world_z);
					const float pitch = glm::atan(world_normal.y, world_normal.z);
					const float yaw = glm::atan(world_normal.y, world_normal.x);

					const uint32_t i = x + z * GRASS_COUNT_SIDE;

					vertices[i].pitch = pitch;
					vertices[i].yaw = yaw;
					vertices[i].position = glm::vec3(
						world_x,
						world_y,
						world_z
					);
					vertices[i].gain = 0.0f;
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
			vkCmdDraw(commandBuffer, GRASS_COUNT, 1, 0, 0);
		}

	private:
		void CreateVertexBuffers(void* vertices)
		{
			const uint32_t vertexCount = static_cast<uint32_t>(GRASS_COUNT);

			const VkDeviceSize bufferSize = sizeof(Vertex) * GRASS_COUNT;
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

		Pipeline::Device& device;
		Pipeline::Buffer* vertexBuffer;
	};
}
