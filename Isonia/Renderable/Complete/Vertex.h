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

namespace Isonia::Renderable::Complete
{
	struct Vertex
	{
		glm::vec3 position{};
		glm::vec3 color{};
		glm::vec3 normal{};
		glm::vec2 uv{};

		static constexpr std::vector<VkVertexInputBindingDescription> GetBindingDescriptions()
		{
			std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
			bindingDescriptions[0].binding = 0;
			bindingDescriptions[0].stride = sizeof(Vertex);
			bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescriptions;
		}

		static constexpr std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

			attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
			attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
			attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
			attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

			return attributeDescriptions;
		}

		bool operator==(const Vertex& other) const
		{
			return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
		}
	};
}

namespace std
{
	template <>
	struct hash<Isonia::Renderable::Complete::Vertex>
	{
		size_t operator()(const Isonia::Renderable::Complete::Vertex& vertex) const
		{
			size_t seed = 0;
			Isonia::Utilities::Hash::HashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}