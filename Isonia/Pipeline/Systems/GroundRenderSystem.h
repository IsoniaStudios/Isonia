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
#include "../../Renderable/BuilderXZUniform.h"

// shaders
#include "../../Shaders/Include/Ground/FragShader_frag.h"
#include "../../Shaders/Include/Ground/VertexShader_vert.h"

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

extern Isonia::ECS::Coordinator gCoordinator;

namespace Isonia::Pipeline::Systems
{
	const std::size_t GROUNDS = 100;
	const std::size_t GROUNDS_COUNT = GROUNDS * GROUNDS;

	class GroundRenderSystem
	{
	public:
		GroundRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device)
		{
			CreatePipelineLayout(globalSetLayout);
			CreatePipeline(renderPass);

			grounds = static_cast<Renderable::BuilderXZUniform*>(operator new[](sizeof(Renderable::BuilderXZUniform) * GROUNDS_COUNT));
			for (size_t x = 0; x < GROUNDS; x++)
			{
				for (size_t z = 0; z < GROUNDS; z++)
				{
					new (grounds + x * GROUNDS + z) Renderable::BuilderXZUniform(device, x * 18.0f, z * 18.0f);
				}
			}
		}

		~GroundRenderSystem()
		{
			vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, nullptr);

			for (size_t x = GROUNDS - 1; x >= 0; x--)
			{
				for (size_t z = GROUNDS - 1; z >= 0; z--)
				{
					grounds[x * GROUNDS + z].~BuilderXZUniform();
				}
			}
			operator delete[](grounds);
		}

		GroundRenderSystem(const GroundRenderSystem&) = delete;
		GroundRenderSystem& operator=(const GroundRenderSystem&) = delete;

		void RenderGround(State::FrameInfo& frameInfo)
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

			for (size_t x = 0; x < GROUNDS; x++)
			{
				for (size_t z = 0; z < GROUNDS; z++)
				{
					Renderable::BuilderXZUniform* ground = &grounds[x * GROUNDS + z];

					vkCmdPushConstants(
						frameInfo.commandBuffer,
						pipelineLayout,
						VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
						0,
						sizeof(Renderable::XZPositionalData),
						&(ground->positionalData)
					);
					ground->Bind(frameInfo.commandBuffer);
					ground->Draw(frameInfo.commandBuffer);
				}
			}
		}

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
		{
			VkPushConstantRange pushConstantRange{};
			pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			pushConstantRange.offset = 0;
			pushConstantRange.size = sizeof(Renderable::XZPositionalData);

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
			Pipeline::PixelPipelineTriangleStripConfigInfo(pipelineConfig);
			pipelineConfig.renderPass = renderPass;
			pipelineConfig.pipelineLayout = pipelineLayout;
			pipeline = new Pipeline(
				device,
				Shaders::Ground::VERTEXSHADER_VERT,
				sizeof(Shaders::Ground::VERTEXSHADER_VERT) / sizeof(unsigned char),
				Shaders::Ground::FRAGSHADER_FRAG,
				sizeof(Shaders::Ground::FRAGSHADER_FRAG) / sizeof(unsigned char),
				pipelineConfig
			);
		}

		Device& device;

		Pipeline* pipeline;
		VkPipelineLayout pipelineLayout;

		Renderable::BuilderXZUniform* grounds;
	};
}
