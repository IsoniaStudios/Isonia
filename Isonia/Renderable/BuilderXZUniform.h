#pragma once

// internal
#include "VertexXZUniform.h"
#include "../Pipeline/Buffer.h"
#include "../Pipeline/Device.h"
#include "../Noise/Noise.h"

// std
#include <stdexcept>
#include <bitset>
#include <cmath>

namespace Isonia::Renderable
{
	const float QUAD_SIZE = 0.1f;
	const std::size_t QUADS = 256;
	const std::size_t VERTICES = QUADS + 1;
	const std::size_t VERTICES_COUNT = VERTICES * VERTICES + (VERTICES - 2) * (VERTICES - 1);

	struct XZPositionalData
	{
		XZPositionalData(const float x, const float z) : x(x), z(z) { }
		const float x;
		const float z;
	};

	struct BuilderXZUniform
	{
		VertexXZUniform vertices[VERTICES_COUNT];
		const XZPositionalData positionalData;

		BuilderXZUniform(Pipeline::Device& device, float x, float z) : device(device), positionalData(x, z)
		{
			Noise::Noise noise{};

			for (size_t i = 0; i < VERTICES_COUNT; i++)
			{
				const int strip = CalculateStrip(i);
				const int row = CalculateRow(i, strip);
				const int col = CalculateCol(i, strip);

				const float x = row * QUAD_SIZE + positionalData.x;
				const float z = col * QUAD_SIZE + positionalData.z;

				vertices[i].altitude = noise.GeneratePerlinNoise(69, x, z);
			}
			CreateVertexBuffers();
		}

		~BuilderXZUniform()
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
