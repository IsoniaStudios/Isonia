#pragma once

// internal
#include "../Components/Camera.h"
#include "../Pipeline/Descriptors/Descriptors.h"

// external
#include <vulkan/vulkan.h>

namespace Isonia::State
{
	struct GlobalUbo
	{
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inverseView{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };  // w is intensity
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ -1.f, 3.f, -1.f });
	};

	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		VkDescriptorSet globalDescriptorSet;
		Pipeline::Descriptors::DescriptorPool& descriptorPool;
	};
}
