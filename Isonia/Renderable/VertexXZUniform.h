#pragma once

// external
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <glm/gtx/hash.hpp>

// internal
#include "../Utilities/HashUtility.h"

// std
#include <unordered_map>
#include <cstdint>

namespace Isonia::Renderable
{
	struct VertexXZUniform
	{
		float altitude;
		unsigned short pitch;
		unsigned short yaw;

		VertexXZUniform() : altitude(0.f), pitch(0), yaw(0)
		{
		}

		VertexXZUniform(float altitude, unsigned short pitch, unsigned short yaw)
			: altitude(altitude), pitch(pitch), yaw(yaw)
		{
		}

		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions()
		{
			std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
			bindingDescriptions[0].binding = 0;
			bindingDescriptions[0].stride = sizeof(VertexXZUniform);
			bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescriptions;
		}

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

			attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32_SFLOAT, offsetof(VertexXZUniform, altitude) });
			attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R16_UNORM, offsetof(VertexXZUniform, pitch) });
			attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R16_UNORM, offsetof(VertexXZUniform, yaw) });

			return attributeDescriptions;
		}
	};
}
