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
	static constexpr const float QUAD_SIZE = 4.0f;
	static constexpr const uint32_t QUADS = 32;
	static constexpr const uint32_t VERTICES = QUADS + 1;
	static constexpr const uint32_t UNIQUE_VERTICES_COUNT = VERTICES * VERTICES;
	static constexpr const uint32_t SAMPLE = VERTICES + 2;
	static constexpr const uint32_t SAMPLE_COUNT = SAMPLE * SAMPLE;
	static constexpr const uint32_t VERTICES_COUNT = VERTICES * VERTICES + (VERTICES - 2) * (VERTICES - 1);
	static constexpr const uint32_t TRIANGLE_COUNT = VERTICES_COUNT - 2;

	struct Builder
	{
		glm::vec3 position;

		Builder(Pipeline::Device& device) : device(device)
		{
			// alloc memory
			Vertex* vertices = static_cast<Vertex*>(operator new[](sizeof(Vertex) * VERTICES_COUNT));
			// create the buffers
			CreateVertexBuffers(vertices);
			// cleanup
			delete vertices;
		}

		Builder(Pipeline::Device& device, const Noise::WarpNoise& warpNoise, const Noise::Noise& noise, const float amplitude, const glm::vec3 position) : device(device), position(position)
		{
			// alloc memory
			Vertex* vertices = static_cast<Vertex*>(operator new[](sizeof(Vertex) * VERTICES_COUNT));

			// calculate and assign perlin altitude
			for (uint32_t i = 0; i < VERTICES_COUNT; i++)
			{
				// calculate strip row and col
				const uint32_t strip = CalculateStrip(i);
				const int32_t col = CalculateCol(i, strip); // is x
				const int32_t row = CalculateRow(i, strip); // is z

				// calculate noise and assign
				float z = row * QUAD_SIZE + position.z - QUAD_SIZE;
				float x = col * QUAD_SIZE + position.x - QUAD_SIZE;
				warpNoise.TransformCoordinate(x, z);
				vertices[i].altitude = noise.GenerateNoise(x, z) * amplitude;
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
