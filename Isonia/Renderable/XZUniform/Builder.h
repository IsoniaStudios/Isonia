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

namespace Isonia::Renderable::XZUniform
{
	const float QUAD_SIZE = 1.0f;
	const std::size_t QUADS = 64;
	const std::size_t VERTICES = QUADS + 1;
	const std::size_t UNIQUE_VERTICES_COUNT = VERTICES * VERTICES;
	const std::size_t SAMPLE = VERTICES + 2;
	const std::size_t SAMPLE_COUNT = SAMPLE * SAMPLE;
	const std::size_t VERTICES_COUNT = VERTICES * VERTICES + (VERTICES - 2) * (VERTICES - 1);
	const std::size_t TRIANGLE_COUNT = VERTICES_COUNT - 2;

	struct XZPositionalData
	{
		XZPositionalData(const float x, const float z) : x(x), z(z) { }
		float x;
		float z;
	};

	struct Builder
	{
		XZPositionalData positionalData;

		float sampleAltitudes[SAMPLE][SAMPLE];
		glm::vec3 normals[VERTICES][VERTICES];

		Builder(Pipeline::Device& device, const Noise::WarpNoise& warpNoise, const Noise::Noise& noise, const float amplitude, const float x, const float z) : device(device), positionalData(x, z)
		{
			// alloc memory
			Vertex* vertices = static_cast<Vertex*>(operator new[](sizeof(Vertex) * VERTICES_COUNT));
			
			// calculate perlin
			for (size_t i_z = 0; i_z < SAMPLE; i_z++)
			{
				for (size_t i_x = 0; i_x < SAMPLE; i_x++)
				{
					float z = i_z * QUAD_SIZE + positionalData.z - QUAD_SIZE;
					float x = i_x * QUAD_SIZE + positionalData.x - QUAD_SIZE;
					warpNoise.TransformCoordinate(x, z);
					sampleAltitudes[i_z][i_x] = noise.GenerateNoise(x, z) * amplitude;
				}
			}

			// calculate normal
			for (size_t z = 0; z < VERTICES; z++)
			{
				for (size_t x = 0; x < VERTICES; x++)
				{
					// create the 5 points used in calculating normal
					const glm::vec3 v10 = { -QUAD_SIZE, sampleAltitudes[z + 0][x + 1],	     0.0f };
					const glm::vec3 v01 = {		  0.0f,	sampleAltitudes[z + 1][x + 0], -QUAD_SIZE };
					const glm::vec3 v11 = {		  0.0f,	sampleAltitudes[z + 1][x + 1],	     0.0f };
					const glm::vec3 v21 = {		  0.0f,	sampleAltitudes[z + 1][x + 2],  QUAD_SIZE };
					const glm::vec3 v12 = {  QUAD_SIZE, sampleAltitudes[z + 2][x + 1],		 0.0f };

					// compute normal
					normals[z][x] = Utilities::Math::ComputeSmoothNormalFrom4(v01, v10, v11, v12, v21);
				}
			}

			// assign normal and altitude
			for (size_t i = 0; i < VERTICES_COUNT; i++)
			{
				// calculate strip row and col
				const int strip = CalculateStrip(i);
				const int col = CalculateCol(i, strip); // is x
				const int row = CalculateRow(i, strip); // is z

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
			const size_t min_x = IMath::FloorToInt(local_x);
			const size_t max_x = IMath::CeilToInt(local_x);

			const size_t min_z = IMath::FloorToInt(local_z);
			const size_t max_z = IMath::CeilToInt(local_z);

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
			const size_t max_x = std::min(IMath::CeilToInt(local_x), static_cast<int>(VERTICES));

			const size_t min_z = std::max(IMath::FloorToInt(local_z), 0);
			const size_t max_z = std::min(IMath::CeilToInt(local_z), static_cast<int>(VERTICES));

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
		int CalculateCol(const int index, const int strip) const
		{
			return abs(((strip + 1) / 2) * (int(VERTICES) * 2 - 1) - ((index + (strip % 2)) / 2));
		}
		int CalculateRow(const int index, const int strip) const
		{
			return ((index + strip) % 2) + strip;
		}
		int CalculateStrip(const int index) const
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
