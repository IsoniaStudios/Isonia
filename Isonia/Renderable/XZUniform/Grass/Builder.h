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

namespace Isonia::Renderable::XZUniform::Grass
{
	const float GRASS_SIZE = Utilities::PixelPerfectUtility::PIXELS_PER_UNIT / (16.0 * 2.0);

	struct Builder
	{
		Builder(Pipeline::Device& device, Renderable::XZUniform::Builder* ground, const float density) :
			device(device), pointCountSide(density * QUADS), pointCount(pointCountSide * pointCountSide)
		{
			// alloc memory
			Vertex* vertices = static_cast<Vertex*>(operator new[](sizeof(Vertex) * pointCount));

			Noise::WhiteNoise offsetNoise = {};

			for (size_t z = 0; z < pointCountSide; z++)
			{
				const float mapped_z = z / density;
				const size_t min_z = IMath::FloorToInt(mapped_z);
				const size_t max_z = IMath::CeilToInt(mapped_z);
				const float t_z = mapped_z - min_z;

				for (size_t x = 0; x < pointCountSide; x++)
				{
					const float mapped_x = x / density;
					const size_t min_x = IMath::FloorToInt(mapped_x);
					const size_t max_x = IMath::CeilToInt(mapped_x);
					const float t_x = mapped_x - min_x;

					const float size = QUAD_SIZE / (density * 2.0f);
					const float randX = offsetNoise.GenerateNoise(mapped_x, mapped_z) * size;
					const float randY = 0;
					const float randZ = offsetNoise.GenerateNoise(mapped_x, mapped_z) * size;

					const float world_x = mapped_x * QUAD_SIZE + randX;
					const float world_z = mapped_z * QUAD_SIZE + randZ;

					const size_t i_n_00 = min_x + min_z * VERTICES;
					const size_t i_n_10 = max_x + min_z * VERTICES;
					const size_t i_n_01 = min_x + max_z * VERTICES;
					const size_t i_n_11 = max_x + max_z * VERTICES;
					
					const glm::vec3 n_00 = ground->normals[i_n_00];
					const glm::vec3 n_10 = ground->normals[i_n_10];
					const glm::vec3 n_01 = ground->normals[i_n_01];
					const glm::vec3 n_11 = ground->normals[i_n_11];

					const glm::vec3 n_l_0 = IMath::Lerp(n_00, n_10, t_x);
					const glm::vec3 n_l_1 = IMath::Lerp(n_01, n_11, t_x);

					const glm::vec3 n_l = IMath::Lerp(n_l_0, n_l_1, t_z);

					const float pitch = glm::atan(n_l.y, n_l.z);
					const float yaw = glm::atan(n_l.y, n_l.x);

					const size_t i_y_00 = (min_x + 1) + (min_z + 1) * SAMPLE;
					const size_t i_y_10 = (max_x + 1) + (min_z + 1) * SAMPLE;
					const size_t i_y_01 = (min_x + 1) + (max_z + 1) * SAMPLE;
					const size_t i_y_11 = (max_x + 1) + (max_z + 1) * SAMPLE;

					const float y_00 = ground->sampleAltitudes[i_y_00];
					const float y_10 = ground->sampleAltitudes[i_y_10];
					const float y_01 = ground->sampleAltitudes[i_y_01];
					const float y_11 = ground->sampleAltitudes[i_y_11];

					const float y_l_0 = IMath::Lerp(y_00, y_10, t_x);
					const float y_l_1 = IMath::Lerp(y_01, y_11, t_x);

					const float y_l = IMath::Lerp(y_l_0, y_l_1, t_z);

					const int i = x + z * pointCountSide;

					vertices[i].pitch = pitch;
					vertices[i].yaw = yaw;
					vertices[i].position = glm::vec3(
						world_x + ground->positionalData.x,
						y_l - GRASS_SIZE * Utilities::PixelPerfectUtility::Y_SCALE,
						world_z + ground->positionalData.z
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

		const size_t pointCountSide;
		const size_t pointCount;
		Pipeline::Device& device;
		Pipeline::Buffer* vertexBuffer;
	};
}
