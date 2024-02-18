#pragma once

// internal
#include "../Device.h"
#include "../Pipeline.h"
#include "../Descriptors/Descriptors.h"

#include "../../State/FrameInfo.h"

#include "../../Renderable/XZUniform/Builder.h"

#include "../../Noise/FractalPerlinNoise.h"

// shaders
#include "../../Shaders/Include/Water/FragShader_frag.h"
#include "../../Shaders/Include/Water/VertexShader_vert.h"

namespace Isonia::Pipeline::Systems
{
	class WaterRenderSystem
	{
	public:
		WaterRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device)
		{
			CreatepipelineLayout(globalSetLayout);
			Createpipeline(renderPass);

			Noise::ConstantScalarWarpNoise groundWarpNoise{ 0.05f };
			Noise::FractalPerlinNoise groundNoise{ 69, 3, 2.0f, 0.5f, 0.0f };

			water = new Renderable::XZUniform::Builder(device, groundWarpNoise, groundNoise, 0.5f, 0, 0);
		}

		~WaterRenderSystem()
		{
			delete pipeline;
			vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, nullptr);

			delete water;
		}

		WaterRenderSystem(const WaterRenderSystem&) = delete;
		WaterRenderSystem& operator=(const WaterRenderSystem&) = delete;

		void Render(State::FrameInfo& frameInfo, Components::Camera& camera)
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

			water->positionalData.x = -(Renderable::XZUniform::QUADS * Renderable::XZUniform::QUAD_SIZE * 0.5f);
			water->positionalData.z = -(Renderable::XZUniform::QUADS * Renderable::XZUniform::QUAD_SIZE * 0.5f);
			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(Renderable::XZUniform::XZPositionalData),
				&(water->positionalData)
			);
			water->Bind(frameInfo.commandBuffer);
			water->Draw(frameInfo.commandBuffer);
		}

	private:
		void CreatepipelineLayout(VkDescriptorSetLayout globalSetLayout)
		{
			VkPushConstantRange pushConstantRange{};
			pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			pushConstantRange.offset = 0;
			pushConstantRange.size = sizeof(Renderable::XZUniform::XZPositionalData);

			std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
				globalSetLayout
			};

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

		void Createpipeline(VkRenderPass renderPass)
		{
			assert(pipelineLayout != nullptr && "Cannot create pipeline before a pipeline layout is instantiated");

			PipelineConfigInfo pipelineConfig{};
			Pipeline::PixelPipelineTriangleStripConfigInfo(pipelineConfig);
			pipelineConfig.renderPass = renderPass;
			pipelineConfig.pipelineLayout = pipelineLayout;
			pipeline = Pipeline::Builder(device)
				.AddShaderModule(
					VK_SHADER_STAGE_VERTEX_BIT,
					Shaders::Water::VERTEXSHADER_VERT,
					sizeof(Shaders::Water::VERTEXSHADER_VERT) / sizeof(unsigned char)
				).AddShaderModule(
					VK_SHADER_STAGE_FRAGMENT_BIT,
					Shaders::Water::FRAGSHADER_FRAG,
					sizeof(Shaders::Water::FRAGSHADER_FRAG) / sizeof(unsigned char)
				).CreateGraphicsPipeline(pipelineConfig);
		}

		Device& device;

		Pipeline* pipeline;
		VkPipelineLayout pipelineLayout;

		Renderable::XZUniform::Builder* water;
	};
}
