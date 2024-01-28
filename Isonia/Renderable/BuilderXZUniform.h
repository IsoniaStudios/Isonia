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
				perlinNoise[i] = noise.GeneratePerlinNoise(69, x, z);
			}

			float localNormalNoise[3][3]{};
			for (size_t i = 0; i < VERTICES_COUNT; i++)
			{
				// calculate strip row and col
				const int strip = CalculateStrip(i);
				const int row = CalculateRow(i, strip) + 1;
				const int col = CalculateCol(i, strip) + 1;

				//
				for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
				{
					int rowWO = row + rowOffset;
					for (int colOffset = -1; colOffset <= 1; colOffset++)
					{
						int colWO = col + colOffset;
						localNormalNoise[colOffset + 1][rowOffset + 1] = perlinNoise[rowWO + SAMPLE * colWO]; // unsure why this is flipped but oh well
					}
				}

				// get precalculated perlin noise and set it to altitude
				vertices[i].altitude = localNormalNoise[1][1];

				//glm::vec3 b = { 0.0f, vertices[i].altitude, 0.0f };


				//glm::vec3 n0 = CalculateMiddleVertexNormal3D(glm::vec3(-QUAD_SIZE, localNormalNoise[1][0], 0.0), b, glm::vec3(+QUAD_SIZE, localNormalNoise[1][2], 0.0));
				//glm::vec3 norm = glm::normalize(n0);
				//vertices[i].normal = norm;
				vertices[i].normal = glm::vec3(1.0f, 1.0f, 1.0f);
			}
			/*
			// calculate per triangle normal
			glm::vec3* triangleNorms = static_cast<glm::vec3*>(operator new[](sizeof(glm::vec3) * TRIANGLE_COUNT));
			for (size_t i = 0; i < TRIANGLE_COUNT; i++)
			{
				// get vecs
				glm::vec3 v0{ xzVertices[i + 0].x, vertices[i + 0].altitude, xzVertices[i + 0].y };
				glm::vec3 v1{ xzVertices[i + 1].x, vertices[i + 1].altitude, xzVertices[i + 1].y };
				glm::vec3 v2{ xzVertices[i + 2].x, vertices[i + 2].altitude, xzVertices[i + 2].y };

				// calculate triangle normal
				glm::vec3 cross = glm::cross(v1 - v0, v2 - v0);
				glm::vec3 norm = glm::normalize(cross);

				triangleNorms[i] = norm;
			}

			// calculate smooth normal
			for (size_t i = 0; i < TRIANGLE_COUNT; i++)
			{

				// convert to pitch and yaw
				float pitch = glm::asin(triangleNorms[i].y);
				float yaw = atan2(-triangleNorms[i].x, triangleNorms[i].z);

				vertices[i].pitch = pitch;
				vertices[i].yaw = yaw;
			}

			delete xzVertices;
			delete triangleNorms;
			*/
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
		glm::vec3 CalculateMiddleVertexNormal3D(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) const
		{
			// Calculate vectors AB and BC
			glm::vec3 AB = b - a;
			glm::vec3 BC = c - b;

			// Calculate the average vector
			glm::vec3 averageVector = (AB + BC) / 2.0f;

			return averageVector;
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
