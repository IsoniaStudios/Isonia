#pragma once

// internal
#include "VertexXZUniform.h"
#include "../Pipeline/Buffer.h"
#include "../Pipeline/Device.h"
#include "../Noise/Noise.h"

// external
#include <glm/glm.hpp>

// std
#include <stdexcept>
#include <bitset>
#include <cmath>

namespace Isonia::Renderable
{
	const float QUAD_SIZE = 0.1f;
	const std::size_t QUADS = 256;
	const std::size_t VERTICES = QUADS + 1;
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

	struct BuilderXZUniform
	{
		VertexXZUniform* vertices;
		const XZPositionalData positionalData;

		BuilderXZUniform(Pipeline::Device& device, float x, float z) : device(device), positionalData(x, z)
		{
			// alloc memory
			Noise::Noise noise{};
			vertices = static_cast<VertexXZUniform*>(operator new[](sizeof(VertexXZUniform) * VERTICES_COUNT));
			
			// calculate perlin
			float* perlinNoise = static_cast<float*>(operator new[](sizeof(float) * SAMPLE_COUNT));
			for (size_t i = 0; i < SAMPLE_COUNT; i++)
			{
				// calculate xz
				float x = (i % SAMPLE) * QUAD_SIZE + positionalData.x - QUAD_SIZE;
				float z = (i / SAMPLE) * QUAD_SIZE + positionalData.z - QUAD_SIZE;

				// calculate perlin noise from xz
				perlinNoise[i] = noise.GeneratePerlinNoise(69, x, z) * 0.5f;
			}

			float localY[3][3]{};
			for (size_t i = 0; i < VERTICES_COUNT; i++)
			{
				// calculate strip row and col
				const int strip = CalculateStrip(i);
				const int row = CalculateRow(i, strip) + 1; // is x
				const int col = CalculateCol(i, strip) + 1; // is z

				//
				for (int colOffset = -1; colOffset <= 1; colOffset++)
				{
					int colWO = col + colOffset;
					for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
					{
						int rowWO = row + rowOffset;
						localY[colOffset + 1][rowOffset + 1] = perlinNoise[rowWO + SAMPLE * colWO]; // unsure why this is flipped but oh well
					}
				}

				// get precalculated perlin noise and set it to altitude
				vertices[i].altitude = localY[1][1];

				// [00, 01, 02]
				// [10, 11, 12]
				// [20, 21, 22]
				glm::vec3 v00 = { -QUAD_SIZE, localY[0][0], -QUAD_SIZE };
				glm::vec3 v10 = { -QUAD_SIZE, localY[1][0],       0.0f };
				glm::vec3 v20 = { -QUAD_SIZE, localY[2][0],  QUAD_SIZE };

				glm::vec3 v01 = {       0.0f, localY[0][1], -QUAD_SIZE };
				glm::vec3 v11 = {       0.0f, localY[1][1],       0.0f };
				glm::vec3 v21 = {       0.0f, localY[2][1],  QUAD_SIZE };

				glm::vec3 v02 = {  QUAD_SIZE, localY[0][2], -QUAD_SIZE };
				glm::vec3 v12 = {  QUAD_SIZE, localY[1][2],       0.0f };
				glm::vec3 v22 = {  QUAD_SIZE, localY[2][2],  QUAD_SIZE };

				glm::vec3 n0 = CalculateMiddleVertexNormal3D(v01, v11, v21);
				glm::vec3 n1 = CalculateMiddleVertexNormal3D(v02, v11, v20);
				glm::vec3 n2 = CalculateMiddleVertexNormal3D(v12, v11, v10);
				glm::vec3 n3 = CalculateMiddleVertexNormal3D(v22, v11, v00);

				vertices[i].normal = glm::normalize(n0 + n1 + n2 + n3);
				vertices[i].normal = glm::normalize(n2);
				vertices[i].normal = CalculateSmoothNormal(v00, v01, v02, v10, v11, v12, v20, v21, v22);
			}

			// free locally allocated memory
			delete perlinNoise;

			// create the buffers
			CreateVertexBuffers();
		}

		~BuilderXZUniform()
		{
			delete vertexBuffer;
			delete vertices;
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
		glm::vec3 CalculateSmoothNormal(const glm::vec3& v00, const glm::vec3& v01, const glm::vec3& v02,
										const glm::vec3& v10, const glm::vec3& v11, const glm::vec3& v12,
										const glm::vec3& v20, const glm::vec3& v21, const glm::vec3& v22)
		{
			// Calculate cross products of neighboring vectors
			glm::vec3 crossProduct1 = glm::cross(v00, v11);
			glm::vec3 crossProduct2 = glm::cross(v01, v11);
			glm::vec3 crossProduct3 = glm::cross(v02, v11);
			glm::vec3 crossProduct4 = glm::cross(v10, v11);
			glm::vec3 crossProduct5 = glm::cross(v12, v11);
			glm::vec3 crossProduct6 = glm::cross(v20, v11);
			glm::vec3 crossProduct7 = glm::cross(v21, v11);
			glm::vec3 crossProduct8 = glm::cross(v22, v11);

			// Weighted sum of the neighboring normals
			glm::vec3 smoothNormal = (crossProduct1 +
				crossProduct2 +
				crossProduct3 +
				crossProduct4 +
				crossProduct5 +
				crossProduct6 +
				crossProduct7 +
				crossProduct8);

			// Normalize the result
			smoothNormal = glm::normalize(smoothNormal);

			return smoothNormal;
		}

		glm::vec3 CalculateMiddleVertexNormal3D(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) const
		{
			// calculate vectors AB and BC
			glm::vec3 AB = b - a;
			glm::vec3 BC = c - b;
			// return the sum
			return AB + BC;
		}

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

		void CreateVertexBuffers()
		{
			const VkDeviceSize bufferSize = sizeof(VertexXZUniform) * VERTICES_COUNT;
			const uint32_t vertexSize = sizeof(VertexXZUniform);

			Pipeline::Buffer stagingBuffer{
				device,
				vertexSize,
				VERTICES_COUNT,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			};

			stagingBuffer.Map();
			stagingBuffer.WriteToBuffer((void*)vertices);

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
