// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	constexpr std::vector<VkVertexInputBindingDescription> VertexComplete::getBindingDescriptions()
	{
		return {
			{ 0, sizeof(VertexComplete), VK_VERTEX_INPUT_RATE_VERTEX }
		};
	}

	constexpr std::vector<VkVertexInputAttributeDescription> VertexComplete::getAttributeDescriptions()
	{
		return {
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexComplete, position) },
			{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexComplete, color) },
			{ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexComplete, normal) },
			{ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexComplete, uv) }
		};
	}
}
