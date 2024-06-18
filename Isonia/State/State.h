#pragma once

// internal
#include "../Math/Math.h"

// external
#include <vulkan/vulkan.h>

namespace Isonia::State
{
	struct GlobalUbo
	{
		Math::Matrix4x4 projection{ 1.f };
		Math::Matrix4x4 view{ 1.f };
		Math::Matrix4x4 inverseView{ 1.f };
		Math::Vector4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };  // w is intensity
		Math::Vector3 lightDirection = Math::vec3Normalize(-1.f, 3.f, -1.f);
	};

	struct Clock
	{
		float time{ 0 };
		float frameTime{ 0 };
	};

	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		VkDescriptorSet globalDescriptorSet;
	};
}
