#pragma once

#include "../../ECS/Camera.h"
#include "../Device.h"
#include "../../State/FrameInfo.h"
#include "../../ECS/GameObject.h"
#include "../Pipeline.h"

// std
#include <memory>
#include <vector>

namespace Isonia::Pipeline::Systems
{
	class PointLightSystem
	{
	public:
		PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void Update(State::FrameInfo& frameInfo, State::GlobalUbo& ubo);
		void Render(State::FrameInfo& frameInfo);

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderPass);

		Device& device;

		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;
	};
}
