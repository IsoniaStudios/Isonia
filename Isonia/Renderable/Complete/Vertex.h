#pragma once

// external
#include <vulkan/vulkan.h>

// internal
#include "../../Math/Vector.h"
#include "../../Math/Hash.h"

// std
#include <unordered_map>

namespace Isonia::Renderable::Complete
{
	struct Vertex
	{
		Math::Vector3 position{};
		Math::Vector3 color{};
		Math::Vector3 normal{};
		Math::Vector2 uv{};

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
			Isonia::Math::HashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}
