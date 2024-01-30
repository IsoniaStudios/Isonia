#pragma once

// external
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <glm/gtx/hash.hpp>

// internal
#include "../../Utilities/HashUtility.h"

// std
#include <unordered_map>
#include <cstdint>

namespace Isonia::Renderable::XZUniform
{
	struct Vertex
	{
		float altitude;
		glm::vec3 normal;

		Vertex(float altitude, glm::vec3 normal)
			: altitude(altitude), normal(normal)
		{
		}

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

			attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32_SFLOAT, offsetof(Vertex, altitude) });
			attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });

			return attributeDescriptions;
		}
	};
}
