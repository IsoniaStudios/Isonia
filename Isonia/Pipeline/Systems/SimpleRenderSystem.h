#pragma once

// internal
#include "../Device.h"
#include "../Pipeline.h"
#include "../../State/FrameInfo.h"
#include "../../ECS/System.h"
#include "../../ECS/Coordinator.h"
#include "../../Components/Camera.h"
#include "../../Components/Mesh.h"
#include "../../Components/Transform.h"

// shaders
#include "../../Shaders/Include/Simple/FragShader_frag.h"
#include "../../Shaders/Include/Simple/VertexShader_vert.h"

// external
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <memory>
#include <vector>
#include <array>
#include <cassert>
#include <stdexcept>

extern Isonia::ECS::Coordinator* gCoordinator;

namespace Isonia::Pipeline::Systems
{
	class SimpleRenderSystem : public ECS::System
	{
	public:
		struct SimplePushConstantData
		{
			glm::mat4 modelMatrix{ 1.f };
			glm::mat4 normalMatrix{ 1.f };
		};

		SimpleRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device)
		{
			CreatePipelineLayout(globalSetLayout);
			CreatePipeline(renderPass);
		}

		~SimpleRenderSystem() override
		{
			delete pipeline;
			vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, nullptr);
		}

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void RenderGameObjects(State::FrameInfo& frameInfo)
		{
			pipeline->Bind(frameInfo.commandBuffer);

			vkCmdBindDescriptorSets(
				frameInfo.commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipelineLayout,
				0,
				1,
				&frameInfo.globalDescriptorSet,
				0,
				nullptr
			);
			for (auto const& entity : entities)
			{
				auto* const transform = gCoordinator->GetComponent<Components::Transform>(entity);
				auto* const mesh = gCoordinator->GetComponent<Components::Mesh>(entity);

				SimplePushConstantData push{};
				push.modelMatrix = transform->Mat4();
				push.normalMatrix = transform->NormalMatrix();

				vkCmdPushConstants(
					frameInfo.commandBuffer,
					pipelineLayout,
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
					0,
					sizeof(SimplePushConstantData),
					&push
				);
				mesh->model->Bind(frameInfo.commandBuffer);
				mesh->model->Draw(frameInfo.commandBuffer);
			}
		}

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
		{
			VkPushConstantRange pushConstantRange{};
			pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			pushConstantRange.offset = 0;
			pushConstantRange.size = sizeof(SimplePushConstantData);

			std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
			pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
			pipelineLayoutInfo.pushConstantRangeCount = 1;
			pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
			if (vkCreatePipelineLayout(device.GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create pipeline layout!");
			}
		}

		void CreatePipeline(VkRenderPass renderPass)
		{
			assert(pipelineLayout != nullptr && "Cannot create pipeline before a pipeline layout is instantiated");

			PipelineConfigInfo pipelineConfig{};
			Pipeline::PixelPipelineConfigInfo(pipelineConfig);
			pipelineConfig.renderPass = renderPass;
			pipelineConfig.pipelineLayout = pipelineLayout;
			pipeline = Pipeline::Builder(device)
				.AddShaderModule(
					VK_SHADER_STAGE_VERTEX_BIT,
					Shaders::Simple::VERTEXSHADER_VERT,
					sizeof(Shaders::Simple::VERTEXSHADER_VERT) / sizeof(unsigned char)
				).AddShaderModule(
					VK_SHADER_STAGE_FRAGMENT_BIT,
					Shaders::Simple::FRAGSHADER_FRAG,
					sizeof(Shaders::Simple::FRAGSHADER_FRAG) / sizeof(unsigned char)
				).CreateGraphicsPipeline(pipelineConfig);
		}

		Device& device;

		Pipeline* pipeline;
		VkPipelineLayout pipelineLayout;
	};
}
