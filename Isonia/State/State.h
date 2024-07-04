#pragma once

// internal
#include "../Math/Math.h"

// external
#include <vulkan/vulkan.h>
#include <string>

namespace Isonia::State
{
	struct GlobalUbo
	{
		Math::Matrix4x4 projection{ 1.f };
		Math::Matrix4x4 view{ 1.f };
		Math::Matrix4x4 inverse_view{ 1.f };
		Math::Vector4 ambient_light_color{ 1.f, 1.f, 1.f, .02f };  // w is intensity
		Math::Vector3 light_direction = Math::vec3Normalize(-1.f, 3.f, -1.f);
	};

	struct Clock
	{
		float time_s{ 0 };
		float frame_time_s{ 0 };
	};

	struct FrameInfo
	{
		int frame_index;
		float frame_time_s;
		VkCommandBuffer command_buffer;
		VkDescriptorSet global_descriptor_set;
	};

	struct Keyboard
	{
		static const unsigned int max_keyboard_keys = 512;
		unsigned char key_state[max_keyboard_keys * 2u];
		unsigned char* current_key_state;
		unsigned char* previous_key_state;

		Keyboard()
			: current_key_state(key_state), previous_key_state(key_state + max_keyboard_keys)
		{
			memset(key_state, 0, sizeof(key_state));
		}
	};

	bool GrowHeap(unsigned long long size);
}
