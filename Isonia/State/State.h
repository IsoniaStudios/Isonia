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
		Math::Vector4 ambient_light_color{ 1.f, 1.f, 1.f, .02f }; 
		Math::Vector3 light_direction = Math::vec3Normalize(-1.f, 3.f, -1.f);

		GlobalUbo() = default;
		GlobalUbo(const GlobalUbo&) = delete;
		GlobalUbo& operator=(const GlobalUbo&) = delete;
	};

	struct Clock
	{
		float time_s{ 0 };
		float frame_time_s{ 0 };

		Clock() = default;
		Clock(const Clock&) = delete;
		Clock& operator=(const Clock&) = delete;
	};

	struct FrameInfo
	{
		int frame_index;
		float frame_time_s;
		VkCommandBuffer command_buffer;
		VkDescriptorSet global_descriptor_set;

		FrameInfo(int frame_index, float frame_time_s, VkCommandBuffer command_buffer, VkDescriptorSet global_descriptor_set)
			: frame_index(frame_index), frame_time_s(frame_time_s), command_buffer(command_buffer), global_descriptor_set(global_descriptor_set)
		{

		}

		FrameInfo() = delete;
		FrameInfo(const FrameInfo&) = delete;
		FrameInfo& operator=(const FrameInfo&) = delete;
	};

	struct Keyboard
	{
		static const unsigned int max_keyboard_keys = 512;
		unsigned char current_key_state[max_keyboard_keys];
		unsigned char previous_key_state[max_keyboard_keys];

		Keyboard()
		{
			memset(current_key_state, 0, sizeof(current_key_state));
		}

		Keyboard(const Keyboard&) = delete;
		Keyboard& operator=(const Keyboard&) = delete;
	};

	bool GrowHeap(unsigned long long size);
}
