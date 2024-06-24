// internal
#include "Renderable.h"

namespace Isonia::Renderable
{
	VkVertexInputBindingDescription* VertexComplete::getBindingDescriptions()
	{
		return new VkVertexInputBindingDescription[]{
			{ 0, sizeof(VertexComplete), VK_VERTEX_INPUT_RATE_VERTEX }
		};
	}
	const unsigned int VertexComplete::getBindingDescriptionsCount()
	{
		return 1u;
	}
	VkVertexInputAttributeDescription* VertexComplete::getAttributeDescriptions()
	{
		return new VkVertexInputAttributeDescription[]{
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexComplete, position) },
			{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexComplete, color) },
			{ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexComplete, normal) },
			{ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexComplete, uv) }
		};
	}
	const unsigned int VertexComplete::getAttributeDescriptionsCount()
	{
		return 4u;
	}

	VkVertexInputBindingDescription* VertexPosition::getBindingDescriptions()
	{
		return new VkVertexInputBindingDescription[]{
			{ 0, sizeof(VertexPosition), VK_VERTEX_INPUT_RATE_VERTEX }
		};
	}
	const unsigned int VertexPosition::getBindingDescriptionsCount()
	{
		return 1u;
	}
	VkVertexInputAttributeDescription* VertexPosition::getAttributeDescriptions()
	{
		return new VkVertexInputAttributeDescription[]{
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexPosition, position) }
		};
	}
	const unsigned int VertexPosition::getAttributeDescriptionsCount()
	{
		return 1u;
	}

	VkVertexInputBindingDescription* VertexXZUniform::getBindingDescriptions()
	{
		return new VkVertexInputBindingDescription[]{
			{ 0, sizeof(VertexXZUniform), VK_VERTEX_INPUT_RATE_VERTEX }
		};
	}
	const unsigned int VertexXZUniform::getBindingDescriptionsCount()
	{
		return 1u;
	}
	VkVertexInputAttributeDescription* VertexXZUniform::getAttributeDescriptions()
	{
		return new VkVertexInputAttributeDescription[]{
			{ 0, 0, VK_FORMAT_R32_SFLOAT, offsetof(VertexXZUniform, altitude) }
		};
	}
	const unsigned int VertexXZUniform::getAttributeDescriptionsCount()
	{
		return 1u;
	}

	VkVertexInputBindingDescription* VertexXZUniformN::getBindingDescriptions()
	{
		return new VkVertexInputBindingDescription[]{
			{0, sizeof(VertexXZUniformN), VK_VERTEX_INPUT_RATE_VERTEX}
		};
	}
	const unsigned int VertexXZUniformN::getBindingDescriptionsCount()
	{
		return 1u;
	}
	VkVertexInputAttributeDescription* VertexXZUniformN::getAttributeDescriptions()
	{
		return new VkVertexInputAttributeDescription[]{
			{ 0, 0, VK_FORMAT_R32_SFLOAT, offsetof(VertexXZUniformN, altitude) },
			{ 1, 0, VK_FORMAT_R32_SFLOAT, offsetof(VertexXZUniformN, pitch) },
			{ 2, 0, VK_FORMAT_R32_SFLOAT, offsetof(VertexXZUniformN, yaw) },
		};
	}
	const unsigned int VertexXZUniformN::getAttributeDescriptionsCount()
	{
		return 3u;
	}

	VkVertexInputBindingDescription* VertexXZUniformNP::getBindingDescriptions()
	{
		return new VkVertexInputBindingDescription[]{
			{0, sizeof(VertexXZUniformNP), VK_VERTEX_INPUT_RATE_VERTEX}
		};
	}
	const unsigned int VertexXZUniformNP::getBindingDescriptionsCount()
	{
		return 1u;
	}
	VkVertexInputAttributeDescription* VertexXZUniformNP::getAttributeDescriptions()
	{
		return new VkVertexInputAttributeDescription[]{
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexXZUniformNP, position) },
			{ 1, 0, VK_FORMAT_R32_SFLOAT, offsetof(VertexXZUniformNP, pitch) },
			{ 2, 0, VK_FORMAT_R32_SFLOAT, offsetof(VertexXZUniformNP, yaw) },
			{ 3, 0, VK_FORMAT_R32_SFLOAT, offsetof(VertexXZUniformNP, gain) }
		};
	}
	const unsigned int VertexXZUniformNP::getAttributeDescriptionsCount()
	{
		return 4u;
	}
}
