// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	std::vector<VkVertexInputBindingDescription> VertexComplete::getBindingDescriptions()
	{
		return {
			{ 0, sizeof(VertexComplete), VK_VERTEX_INPUT_RATE_VERTEX }
		};
	}
	std::vector<VkVertexInputAttributeDescription> VertexComplete::getAttributeDescriptions()
	{
		return {
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexComplete, position) },
			{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexComplete, color) },
			{ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexComplete, normal) },
			{ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexComplete, uv) }
		};
	}

	std::vector<VkVertexInputBindingDescription> VertexPosition::getBindingDescriptions()
	{
		return {
			{ 0, sizeof(VertexPosition), VK_VERTEX_INPUT_RATE_VERTEX }
		};
	}
	std::vector<VkVertexInputAttributeDescription> VertexPosition::getAttributeDescriptions()
	{
		return {
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPosition, position) }
		};
	}

	std::vector<VkVertexInputBindingDescription> VertexXZUniform::getBindingDescriptions()
	{
		return {
			{ 0, sizeof(VertexXZUniform), VK_VERTEX_INPUT_RATE_VERTEX }
		};
	}
	std::vector<VkVertexInputAttributeDescription> VertexXZUniform::getAttributeDescriptions()
	{
		return {
			{ 0, 0, VK_FORMAT_R32_SFLOAT, offsetof(VertexXZUniform, altitude) }
		};
	}

	std::vector<VkVertexInputBindingDescription> VertexXZUniformN::getBindingDescriptions()
	{
		return {
			{0, sizeof(VertexXZUniformN), VK_VERTEX_INPUT_RATE_VERTEX}
		};
	}
	std::vector<VkVertexInputAttributeDescription> VertexXZUniformN::getAttributeDescriptions()
	{
		return {
			{ 0, 0, VK_FORMAT_R32_SFLOAT, offsetof(VertexXZUniformN, altitude) },
			{ 1, 0, VK_FORMAT_R32_SFLOAT, offsetof(VertexXZUniformN, pitch) },
			{ 2, 0, VK_FORMAT_R32_SFLOAT, offsetof(VertexXZUniformN, yaw) },
		};
	}

	std::vector<VkVertexInputBindingDescription> VertexXZUniformNP::getBindingDescriptions()
	{
		return {
			{0, sizeof(VertexXZUniformNP), VK_VERTEX_INPUT_RATE_VERTEX}
		};
	}
	std::vector<VkVertexInputAttributeDescription> VertexXZUniformNP::getAttributeDescriptions()
	{
		return {
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexXZUniformNP, position) },
			{ 1, 0, VK_FORMAT_R32_SFLOAT, offsetof(VertexXZUniformNP, pitch) },
			{ 2, 0, VK_FORMAT_R32_SFLOAT, offsetof(VertexXZUniformNP, yaw) },
			{ 3, 0, VK_FORMAT_R32_SFLOAT, offsetof(VertexXZUniformNP, gain) }
		};
	}
}
