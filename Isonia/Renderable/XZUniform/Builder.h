#pragma once

// internal
#include "Vertex.h"
#include "../../Noise/Noise.h"
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
	const float QUAD_SIZE = 0.25f;
	const std::size_t QUADS = 256;
	const std::size_t VERTICES = QUADS + 1;
	const std::size_t UNIQUE_VERTICES_COUNT = VERTICES * VERTICES;
	const std::size_t SAMPLE = VERTICES + 2;
	const std::size_t SAMPLE_COUNT = SAMPLE * SAMPLE;
	const std::size_t VERTICES_COUNT = VERTICES * VERTICES + (VERTICES - 2) * (VERTICES - 1);
	const std::size_t TRIANGLE_COUNT = VERTICES_COUNT - 2;

	struct XZPositionalData
	{
		XZPositionalData(const float x, const float z) : x(x), z(z) { }
		const float x;
		const float z;
	};

	struct Builder
	{
		const XZPositionalData positionalData;

		float* sampleAltitudes;
		glm::vec3* normals;

		Builder(Noise::Noise& noise, Pipeline::Device& device, const float x, const float z) : device(device), positionalData(x, z)
		{
			// alloc memory
			Vertex* vertices = static_cast<Vertex*>(operator new[](sizeof(Vertex) * VERTICES_COUNT));
			
			// calculate perlin
			sampleAltitudes = static_cast<float*>(operator new[](sizeof(float) * SAMPLE_COUNT));
			for (size_t i = 0; i < SAMPLE_COUNT; i++)
			{
				// calculate xz
				const float x = (i % SAMPLE) * QUAD_SIZE + positionalData.x - QUAD_SIZE;
				const float z = (i / SAMPLE) * QUAD_SIZE + positionalData.z - QUAD_SIZE;

				// calculate perlin noise from xz
				sampleAltitudes[i] = noise.GenerateFractalNoise(x, z) * 10.0f;
			}

			// calculate normal
			normals = static_cast<glm::vec3*>(operator new[](sizeof(glm::vec3) * UNIQUE_VERTICES_COUNT));
			for (size_t i = 0; i < UNIQUE_VERTICES_COUNT; i++)
			{
				// calculate xz
				const size_t x = i % VERTICES;
				const size_t z = i / VERTICES;

				// create the 5 points used in calculating normal
				const glm::vec3 v10 = { -QUAD_SIZE, sampleAltitudes[(x + 1) + SAMPLE * (z + 0)], 0.0f };
				const glm::vec3 v01 = { 0.0f,		sampleAltitudes[(x + 0) + SAMPLE * (z + 1)], -QUAD_SIZE };
				const glm::vec3 v11 = { 0.0f,		sampleAltitudes[(x + 1) + SAMPLE * (z + 1)], 0.0f };
				const glm::vec3 v21 = { 0.0f,		sampleAltitudes[(x + 2) + SAMPLE * (z + 1)], QUAD_SIZE };
				const glm::vec3 v12 = { QUAD_SIZE,  sampleAltitudes[(x + 1) + SAMPLE * (z + 2)], 0.0f };

				// compute normal
				normals[i] = Utilities::Math::ComputeSmoothNormalFrom4(v01, v10, v11, v12, v21);
			}

			// assign normal and altitude
			for (size_t i = 0; i < VERTICES_COUNT; i++)
			{
				// calculate strip row and col
				const int strip = CalculateStrip(i);
				const int col = CalculateCol(i, strip); // is x
				const int row = CalculateRow(i, strip); // is z

				// get precalculated perlin noise and set it to altitude
				vertices[i].altitude = sampleAltitudes[(col + 1) + SAMPLE * (row + 1)];

				// get precalculated perlin normal
				const glm::vec3 normal = normals[col + VERTICES * row];

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
			delete sampleAltitudes;
			delete normals;
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
