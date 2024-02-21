#pragma once

// internal
#include "../Device.h"
#include "../Pipeline.h"
#include "../Descriptors/Descriptors.h"

#include "../../State/FrameInfo.h"

#include "../../ECS/System.h"

#include "../../Renderable/Position/Builder.h"

// shaders
#include "../../Shaders/Include/Billboard/FragShader_frag.h"
#include "../../Shaders/Include/Billboard/VertexShader_vert.h"
#include "../../Shaders/Include/Billboard/GeomShader_geom.h"

// std
#include <memory>
#include <vector>
#include <array>
#include <cassert>
#include <stdexcept>
#include <numeric>

namespace Isonia::Pipeline::Systems
{
	class DebuggerRenderSystem
	{
	public:
		DebuggerRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device)
		{
			CreatePipelineLayout(globalSetLayout);
			CreatePipeline(renderPass);

			debugger = new Renderable::Position::Builder(device);
		}

		~DebuggerRenderSystem()
		{
			delete pipeline;
			vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, nullptr);

			delete debugger;
		}

		DebuggerRenderSystem(const DebuggerRenderSystem&) = delete;
		DebuggerRenderSystem& operator=(const DebuggerRenderSystem&) = delete;

		void Render(State::FrameInfo& frameInfo)
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

			debugger->Bind(frameInfo.commandBuffer);
			debugger->Draw(frameInfo.commandBuffer);
		}

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
		{
			const constexpr uint32_t descriptorSetLayoutsLength = 1;
			const VkDescriptorSetLayout descriptorSetLayouts[descriptorSetLayoutsLength]{
				globalSetLayout
			};

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = descriptorSetLayoutsLength;
			pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts;
			pipelineLayoutInfo.pushConstantRangeCount = 0;
			pipelineLayoutInfo.pPushConstantRanges = nullptr;
			if (vkCreatePipelineLayout(device.GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create pipeline layout!");
			}
		}

		void CreatePipeline(VkRenderPass renderPass)
		{
			assert(pipelineLayout != nullptr && "Cannot create pipeline before a pipeline layout is instantiated");

			PipelineConfigInfo pipelineConfig{};
			PixelPipelinePointListConfigInfo(pipelineConfig);
			pipelineConfig.renderPass = renderPass;
			pipelineConfig.pipelineLayout = pipelineLayout;
			pipeline = Pipeline::Builder(device)
				.AddShaderModule(
					VK_SHADER_STAGE_GEOMETRY_BIT,
					Shaders::Billboard::GEOMSHADER_GEOM,
					sizeof(Shaders::Billboard::GEOMSHADER_GEOM) / sizeof(unsigned char)
				).AddShaderModule(
					VK_SHADER_STAGE_VERTEX_BIT,
					Shaders::Billboard::VERTEXSHADER_VERT,
					sizeof(Shaders::Billboard::VERTEXSHADER_VERT) / sizeof(unsigned char)
				).AddShaderModule(
					VK_SHADER_STAGE_FRAGMENT_BIT,
					Shaders::Billboard::FRAGSHADER_FRAG,
					sizeof(Shaders::Billboard::FRAGSHADER_FRAG) / sizeof(unsigned char)
				).CreateGraphicsPipeline(pipelineConfig);
		}

		static void PixelPipelinePointListConfigInfo(PipelineConfigInfo& configInfo)
		{
			Pipeline::PixelPipelineConfigInfo(configInfo);

			configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

			configInfo.bindingDescriptions = Renderable::Position::Vertex::GetBindingDescriptions();
			configInfo.attributeDescriptions = Renderable::Position::Vertex::GetAttributeDescriptions();
		}

		Device& device;

		Pipeline* pipeline;
		VkPipelineLayout pipelineLayout;

		Renderable::Position::Builder* debugger;
	};
}
