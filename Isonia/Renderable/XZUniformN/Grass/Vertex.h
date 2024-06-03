#pragma once

// external
#include <vulkan/vulkan.h>

// internal
#include "../../Math/Vector.h"
#include "../../Math/Hash.h"

// std
#include <unordered_map>
#include <cstdint>

namespace Isonia::Renderable::XZUniformN::Grass
{
	struct Vertex
	{
		Math::Vector3 position;
		float pitch;
		float yaw;
		float gain;

		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions()
		{
			std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
			bindingDescriptions[0].binding = 0;
			bindingDescriptions[0].stride = sizeof(Vertex);
			bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescriptions;
		}

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

			attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
			attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32_SFLOAT, offsetof(Vertex, pitch) });
			attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32_SFLOAT, offsetof(Vertex, yaw) });
			attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32_SFLOAT, offsetof(Vertex, gain) });

			return attributeDescriptions;
		}
	};
}
