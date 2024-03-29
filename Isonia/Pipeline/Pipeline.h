#pragma once

// internal
#include "Device.h"
#include "PipelineConfigInfo.h"
#include "../Renderable/Complete/Model.h"
#include "../Renderable/Complete/Vertex.h"
#include "../Renderable/XZUniform/Vertex.h"
#include "../Renderable/XZUniformN/Vertex.h"

// std
#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Isonia::Pipeline
{
	class Pipeline
	{
	public:
		class Builder
		{
		public:
			Builder(Device& device) : device(device)
			{
			};

			Builder& AddShaderModule(VkShaderStageFlagBits stage, const unsigned char* const code, const size_t size)
			{
				VkShaderModule shaderModule = CreateShaderModule(code, size);

				VkPipelineShaderStageCreateInfo shaderStage;
				shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStage.stage = stage;
				shaderStage.module = shaderModule;
				shaderStage.pName = "main";
				shaderStage.flags = 0;
				shaderStage.pNext = nullptr;
				shaderStage.pSpecializationInfo = nullptr;

				shaderStages.push_back(shaderStage);

				return *this;
			}

			Pipeline* CreateGraphicsPipeline(const PipelineConfigInfo& configInfo) const
			{
				return new Pipeline(device, shaderStages, configInfo);
			}

		private:
			VkShaderModule CreateShaderModule(const unsigned char* const code, const size_t size)
			{
				VkShaderModuleCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.codeSize = size;
				createInfo.pCode = reinterpret_cast<const uint32_t*>(code);

				VkShaderModule shaderModule{};
				if (vkCreateShaderModule(device.GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to create shader module");
				}
				return shaderModule;
			}

			Device& device;
			std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
		};


		Pipeline(Device& device, std::vector<VkPipelineShaderStageCreateInfo> shaderStages, const PipelineConfigInfo& configInfo) : device(device)
		{
			CreateGraphicsPipeline(shaderStages, configInfo);
		}

		~Pipeline()
		{
			for (auto const& shaderStage : shaderStages)
			{
				vkDestroyShaderModule(device.GetDevice(), shaderStage.module, nullptr);
			}
			vkDestroyPipeline(device.GetDevice(), graphicsPipeline, nullptr);
		}

		VkShaderStageFlags GetStageFlags() const
		{
			return stageFlags;
		}

		Pipeline() = default;
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		void Bind(VkCommandBuffer commandBuffer)
		{
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		}

		static constexpr void PixelPipelineTriangleStripConfigInfo(PipelineConfigInfo& configInfo)
		{
			Pipeline::DefaultPipelineConfigInfo(configInfo);
			Pipeline::MakePixelPerfectConfigInfo(configInfo);
			Pipeline::MakeTriangleStripConfigInfo(configInfo);

			configInfo.bindingDescriptions = Renderable::XZUniform::Vertex::GetBindingDescriptions();
			configInfo.attributeDescriptions = Renderable::XZUniform::Vertex::GetAttributeDescriptions();
		}

		static constexpr void PixelPipelineTriangleStripNormalConfigInfo(PipelineConfigInfo& configInfo)
		{
			Pipeline::DefaultPipelineConfigInfo(configInfo);
			Pipeline::MakePixelPerfectConfigInfo(configInfo);
			Pipeline::MakeTriangleStripConfigInfo(configInfo);

			configInfo.bindingDescriptions = Renderable::XZUniformN::Vertex::GetBindingDescriptions();
			configInfo.attributeDescriptions = Renderable::XZUniformN::Vertex::GetAttributeDescriptions();
		}

		static constexpr void PixelPipelineConfigInfo(PipelineConfigInfo& configInfo)
		{
			Pipeline::DefaultPipelineConfigInfo(configInfo);
			Pipeline::MakePixelPerfectConfigInfo(configInfo);
		}

		static constexpr void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo)
		{
			configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

			configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			configInfo.viewportInfo.viewportCount = 1;
			configInfo.viewportInfo.pViewports = nullptr;
			configInfo.viewportInfo.scissorCount = 1;
			configInfo.viewportInfo.pScissors = nullptr;

			configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
			configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
			configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
			configInfo.rasterizationInfo.lineWidth = 1.0f;
			configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
			configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
			configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
			configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
			configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

			configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
			configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
			configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
			configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
			configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

			configInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
			configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
			configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
			configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
			configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
			configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
			configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

			configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
			configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
			configInfo.colorBlendInfo.attachmentCount = 1;
			configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
			configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
			configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
			configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
			configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

			configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
			configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
			configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
			configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
			configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
			configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
			configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
			configInfo.depthStencilInfo.front = {};  // Optional
			configInfo.depthStencilInfo.back = {};   // Optional

			configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
			configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
			configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
			configInfo.dynamicStateInfo.flags = 0;

			configInfo.bindingDescriptions = Renderable::Complete::Vertex::GetBindingDescriptions();
			configInfo.attributeDescriptions = Renderable::Complete::Vertex::GetAttributeDescriptions();
		}

		static constexpr void MakePixelPerfectConfigInfo(PipelineConfigInfo& configInfo)
		{
			configInfo.rasterizationInfo.lineWidth = 1.0f;

			configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;

			configInfo.colorBlendAttachment.blendEnable = VK_FALSE;

			configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
			configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		}

		static constexpr void MakeTransparentConfigInfo(PipelineConfigInfo& configInfo)
		{
			configInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			configInfo.colorBlendAttachment.blendEnable = VK_TRUE;
			configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		}

		static constexpr void MakeTriangleStripConfigInfo(PipelineConfigInfo& configInfo)
		{
			configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		}

	private:
		void CreateGraphicsPipeline(std::vector<VkPipelineShaderStageCreateInfo> shaderStages, const PipelineConfigInfo& configInfo)
		{
			assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
			assert(configInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo");

			auto& bindingDescriptions = configInfo.bindingDescriptions;
			auto& attributeDescriptions = configInfo.attributeDescriptions;
			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
			vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

			this->shaderStages = shaderStages;
			for (auto const& shaderStage : shaderStages)
			{
				stageFlags |= shaderStage.stage;
			}

			VkGraphicsPipelineCreateInfo pipelineInfo{};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
			pipelineInfo.pStages = shaderStages.data();
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
			pipelineInfo.pViewportState = &configInfo.viewportInfo;
			pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
			pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
			pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
			pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
			pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;

			pipelineInfo.layout = configInfo.pipelineLayout;
			pipelineInfo.renderPass = configInfo.renderPass;
			pipelineInfo.subpass = configInfo.subpass;

			pipelineInfo.basePipelineIndex = -1;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

			if (vkCreateGraphicsPipelines(device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create graphics pipeline");
			}
		}

		Device& device;
		VkPipeline graphicsPipeline;

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		VkShaderStageFlags stageFlags{};
	};
}
