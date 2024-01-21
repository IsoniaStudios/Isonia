#pragma once

// internal
#include "../../Components/Camera.h"
#include "../Device.h"
#include "../../State/FrameInfo.h"
#include "../Pipeline.h"
#include "../../ECS/System.h"

// std
#include <memory>
#include <vector>

namespace Isonia::Pipeline::Systems
{
	class SimpleRenderSystem : public ECS::System
	{
	public:
		SimpleRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void RenderGameObjects(State::FrameInfo& frameInfo);

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderPass);

		Device& device;

		Pipeline* pipeline;
		VkPipelineLayout pipelineLayout;
	};
}
