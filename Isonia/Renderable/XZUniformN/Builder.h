#pragma once

// internal
#include "Vertex.h"
#include "../../Noise/Noise.h"
#include "../../Noise/WarpNoise.h"
#include "../../Pipeline/Buffer.h"
#include "../../Pipeline/Device.h"

// external
#include <glm/glm.hpp>

// std
#include <stdexcept>
#include <bitset>
#include <cmath>

namespace Isonia::Renderable::XZUniformN
{
	static constexpr const float QUAD_SIZE = 1.0f;
	static constexpr const uint32_t QUADS = 64;
	static constexpr const uint32_t VERTICES = QUADS + 1;
	static constexpr const uint32_t UNIQUE_VERTICES_COUNT = VERTICES * VERTICES;
	static constexpr const uint32_t SAMPLE = VERTICES + 2;
	static constexpr const uint32_t SAMPLE_COUNT = SAMPLE * SAMPLE;
	static constexpr const uint32_t VERTICES_COUNT = VERTICES * VERTICES + (VERTICES - 2) * (VERTICES - 1);
	static constexpr const uint32_t TRIANGLE_COUNT = VERTICES_COUNT - 2;

	struct XZPositionalData
	{
		XZPositionalData(const float x, const float z) : x(x), z(z) { }
		const float x;
		const float z;
	};

	struct Builder
	{
		const XZPositionalData positionalData;

		float sampleAltitudes[SAMPLE][SAMPLE];
		glm::vec3 normals[VERTICES][VERTICES];

		Builder(Pipeline::Device& device, const Noise::WarpNoise& warpNoise, const Noise::Noise& noise, const float amplitude, const float x, const float z) : device(device), positionalData(x, z)
		{
			// alloc memory
			Vertex* vertices = static_cast<Vertex*>(operator new[](sizeof(Vertex) * VERTICES_COUNT));
			
			// calculate perlin
			for (uint32_t i_z = 0; i_z < SAMPLE; i_z++)
			{
				for (uint32_t i_x = 0; i_x < SAMPLE; i_x++)
				{
					float z = i_z * QUAD_SIZE + positionalData.z - QUAD_SIZE;
					float x = i_x * QUAD_SIZE + positionalData.x - QUAD_SIZE;
					warpNoise.TransformCoordinate(x, z);
					sampleAltitudes[i_z][i_x] = noise.GenerateNoise(x, z) * amplitude;
				}
			}

			for (uint32_t z = 1; z < SAMPLE - 1; z++)
			{
				for (uint32_t x = 1; x < SAMPLE - 1; x++)
				{
					normals[z - 1][x - 1] = Utilities::Math::ComputeSmoothNormalFrom4(*sampleAltitudes, QUAD_SIZE, QUAD_SIZE, SAMPLE, SAMPLE, z, x);
				}
			}

			// assign normal and altitude
			for (uint32_t i = 0; i < VERTICES_COUNT; i++)
			{
				// calculate strip row and col
				const int32_t strip = CalculateStrip(i);
				const int32_t col = CalculateCol(i, strip); // is x
				const int32_t row = CalculateRow(i, strip); // is z

				// get precalculated perlin noise and set it to altitude
				vertices[i].altitude = sampleAltitudes[row + 1][col + 1];

				// get precalculated perlin normal
				const glm::vec3 normal = normals[row][col];

				// calculate pitch and yaw
				vertices[i].pitch = glm::atan(normal.y, normal.z);
				vertices[i].yaw = glm::atan(normal.y, normal.x);
			}

			// create the buffers
			CreateVertexBuffers(vertices);

			// cleanup
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
			vkCmdDraw(commandBuffer, VERTICES_COUNT, 1, 0, 0);
		}

		float MapWorldToHeight(const float world_x, const float world_z) const
		{
			// get local
			const float local_x = world_x + QUAD_SIZE - positionalData.x;
			const float local_z = world_z + QUAD_SIZE - positionalData.z;

			// map local to indices
			const uint32_t min_x = IMath::FloorToInt(local_x);
			const uint32_t max_x = IMath::CeilToInt(local_x);

			const uint32_t min_z = IMath::FloorToInt(local_z);
			const uint32_t max_z = IMath::CeilToInt(local_z);

			// get heights from indices
			const float h_00 = sampleAltitudes[min_z][min_x];
			const float h_10 = sampleAltitudes[min_z][max_x];
			const float h_01 = sampleAltitudes[max_z][min_x];
			const float h_11 = sampleAltitudes[max_z][max_x];

			// get t for lerp
			const float t_x = local_x - min_x;
			const float t_z = local_z - min_z;

			// lerp between and return
			const float h_l_0 = IMath::Lerp(h_00, h_10, t_x);
			const float h_l_1 = IMath::Lerp(h_01, h_11, t_x);

			return IMath::Lerp(h_l_0, h_l_1, t_z);
		}

		glm::vec3 MapWorldToNormal(const float world_x, const float world_z) const
		{
			// get local
			const float local_x = world_x - positionalData.x;
			const float local_z = world_z - positionalData.z;

			// map local to indices
			const size_t min_x = std::max(IMath::FloorToInt(local_x), 0);
			const size_t max_x = std::min(IMath::CeilToInt(local_x), static_cast<int32_t>(VERTICES));

			const size_t min_z = std::max(IMath::FloorToInt(local_z), 0);
			const size_t max_z = std::min(IMath::CeilToInt(local_z), static_cast<int32_t>(VERTICES));

			// get normals from indices
			const glm::vec3 n_00 = normals[min_z][min_x];
			const glm::vec3 n_10 = normals[min_z][max_x];
			const glm::vec3 n_01 = normals[max_z][min_x];
			const glm::vec3 n_11 = normals[max_z][max_x];

			// get t for lerp
			const float t_x = local_x - min_x;
			const float t_z = local_z - min_z;

			// lerp between and return
			const glm::vec3 n_l_0 = IMath::Lerp(n_00, n_10, t_x);
			const glm::vec3 n_l_1 = IMath::Lerp(n_01, n_11, t_x);

			return IMath::Lerp(n_l_0, n_l_1, t_z);
		}

	private:
		int32_t CalculateCol(const int32_t index, const int32_t strip) const
		{
			return abs(((strip + 1) / 2) * (int(VERTICES) * 2 - 1) - ((index + (strip % 2)) / 2));
		}
		int32_t CalculateRow(const int32_t index, const int32_t strip) const
		{
			return ((index + strip) % 2) + strip;
		}
		int32_t CalculateStrip(const int32_t index) const
		{
			return (index - 1) / (int(VERTICES) * 2 - 1);
		}

		void CreateVertexBuffers(void* vertices)
		{
			const VkDeviceSize bufferSize = sizeof(Vertex) * VERTICES_COUNT;
			const uint32_t vertexSize = sizeof(Vertex);

			Pipeline::Buffer stagingBuffer{
				device,
				vertexSize,
				VERTICES_COUNT,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			};

			stagingBuffer.Map();
			stagingBuffer.WriteToBuffer(vertices);

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
