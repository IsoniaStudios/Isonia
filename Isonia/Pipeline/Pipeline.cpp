#include "Pipeline.h"

namespace Isonia::Pipeline
{
	Pipeline::Builder::Builder(Device& device) : m_device(device) { }

	Pipeline::Builder& Pipeline::Builder::addShaderModule(VkShaderStageFlagBits stage, const unsigned char* const code, const size_t size)
	{
		VkShaderModule shader_module = createShaderModule(code, size);

		VkPipelineShaderStageCreateInfo shader_stage;
		shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shader_stage.stage = stage;
		shader_stage.module = shader_module;
		shader_stage.pName = "main";
		shader_stage.flags = 0;
		shader_stage.pNext = nullptr;
		shader_stage.pSpecializationInfo = nullptr;

		m_shader_stages.push_back(shader_stage);

		return *this;
	}

	Pipeline* Pipeline::Builder::createGraphicsPipeline(const PipelineConfigInfo& config_info) const
	{
		return new Pipeline(m_device, m_shader_stages, config_info);
	}

	VkShaderModule Pipeline::Builder::createShaderModule(const unsigned char* const code, const size_t size)
	{
		VkShaderModuleCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = size;
		create_info.pCode = reinterpret_cast<const uint32_t*>(code);

		VkShaderModule shader_module{};
		if (vkCreateShaderModule(m_device.GetDevice(), &create_info, nullptr, &shader_module) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module");
		}
		return shader_module;
	}

	Pipeline::Pipeline(Device& device, std::vector<VkPipelineShaderStageCreateInfo> shader_stages, const PipelineConfigInfo& config_info)
		: m_device(device)
	{
		createGraphicsPipeline(shader_stages, config_info);
	}

	Pipeline::~Pipeline()
	{
		for (auto const& shader_stage : m_shader_stages)
		{
			vkDestroyShaderModule(m_device.GetDevice(), shader_stage.module, nullptr);
		}
		vkDestroyPipeline(m_device.GetDevice(), m_graphics_pipeline, nullptr);
	}

	VkShaderStageFlags Pipeline::getStageFlags() const
	{
		return m_stage_flags;
	}

	void Pipeline::bind(VkCommandBuffer command_buffer)
	{
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);
	}

	static constexpr void Pipeline::pixel_pipeline_triangle_stripConfigInfo(PipelineConfigInfo& config_info)
	{
		defaultPipelineConfigInfo(config_info);
		makePixelPerfectConfigInfo(config_info);
		makeTriangleStripConfigInfo(config_info);

		config_info.bindingDescriptions = Renderable::XZUniform::Vertex::getBindingDescriptions();
		config_info.attributeDescriptions = Renderable::XZUniform::Vertex::getAttributeDescriptions();
	}

	static constexpr void Pipeline::pixelPipelineTriangleStripNormalConfigInfo(PipelineConfigInfo& config_info)
	{
		defaultPipelineConfigInfo(config_info);
		makePixelPerfectConfigInfo(config_info);
		makeTriangleStripConfigInfo(config_info);

		config_info.bindingDescriptions = Renderable::XZUniformN::Vertex::getBindingDescriptions();
		config_info.attributeDescriptions = Renderable::XZUniformN::Vertex::getAttributeDescriptions();
	}

	static constexpr void Pipeline::pixelPipelineConfigInfo(PipelineConfigInfo& config_info)
	{
		defaultPipelineConfigInfo(config_info);
		makePixelPerfectConfigInfo(config_info);
	}

	static constexpr void Pipeline::defaultPipelineConfigInfo(PipelineConfigInfo& config_info)
	{
		config_info.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		config_info.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		config_info.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		config_info.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		config_info.viewportInfo.viewportCount = 1;
		config_info.viewportInfo.pViewports = nullptr;
		config_info.viewportInfo.scissorCount = 1;
		config_info.viewportInfo.pScissors = nullptr;

		config_info.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		config_info.rasterizationInfo.depthClampEnable = VK_FALSE;
		config_info.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		config_info.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		config_info.rasterizationInfo.lineWidth = 1.0f;
		config_info.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		config_info.rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		config_info.rasterizationInfo.depthBiasEnable = VK_FALSE;
		config_info.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
		config_info.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
		config_info.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

		config_info.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		config_info.multisampleInfo.sampleShadingEnable = VK_FALSE;
		config_info.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		config_info.multisampleInfo.minSampleShading = 1.0f;           // Optional
		config_info.multisampleInfo.pSampleMask = nullptr;             // Optional
		config_info.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		config_info.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

		config_info.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		config_info.colorBlendAttachment.blendEnable = VK_FALSE;
		config_info.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		config_info.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		config_info.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		config_info.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		config_info.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		config_info.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

		config_info.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		config_info.colorBlendInfo.logicOpEnable = VK_FALSE;
		config_info.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		config_info.colorBlendInfo.attachmentCount = 1;
		config_info.colorBlendInfo.pAttachments = &config_info.colorBlendAttachment;
		config_info.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		config_info.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		config_info.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		config_info.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

		config_info.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		config_info.depthStencilInfo.depthTestEnable = VK_TRUE;
		config_info.depthStencilInfo.depthWriteEnable = VK_TRUE;
		config_info.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		config_info.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		config_info.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
		config_info.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		config_info.depthStencilInfo.stencilTestEnable = VK_FALSE;
		config_info.depthStencilInfo.front = {};  // Optional
		config_info.depthStencilInfo.back = {};   // Optional

		config_info.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		config_info.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		config_info.dynamicStateInfo.pDynamicStates = config_info.dynamicStateEnables.data();
		config_info.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(config_info.dynamicStateEnables.size());
		config_info.dynamicStateInfo.flags = 0;

		config_info.bindingDescriptions = Renderable::Complete::Vertex::GetBindingDescriptions();
		config_info.attributeDescriptions = Renderable::Complete::Vertex::GetAttributeDescriptions();
	}

	static constexpr void MakePixelPerfectConfigInfo(PipelineConfigInfo& config_info)
	{
		config_info.rasterizationInfo.lineWidth = 1.0f;

		config_info.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		config_info.multisampleInfo.sampleShadingEnable = VK_FALSE;

		config_info.colorBlendAttachment.blendEnable = VK_FALSE;

		config_info.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		config_info.depthStencilInfo.stencilTestEnable = VK_FALSE;
	}

	static constexpr void MakeTransparentConfigInfo(PipelineConfigInfo& config_info)
	{
		config_info.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		config_info.colorBlendAttachment.blendEnable = VK_TRUE;
		config_info.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		config_info.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		config_info.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		config_info.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		config_info.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		config_info.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	}

	static constexpr void MakeTriangleStripConfigInfo(PipelineConfigInfo& config_info)
	{
		config_info.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		config_info.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	}

	void CreateGraphicsPipeline(std::vector<VkPipelineShaderStageCreateInfo> shaderStages, const PipelineConfigInfo& config_info)
	{
		assert(config_info.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
		assert(config_info.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo");

		auto& bindingDescriptions = config_info.bindingDescriptions;
		auto& attributeDescriptions = config_info.attributeDescriptions;
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
		pipelineInfo.pInputAssemblyState = &config_info.inputAssemblyInfo;
		pipelineInfo.pViewportState = &config_info.viewportInfo;
		pipelineInfo.pRasterizationState = &config_info.rasterizationInfo;
		pipelineInfo.pMultisampleState = &config_info.multisampleInfo;
		pipelineInfo.pColorBlendState = &config_info.colorBlendInfo;
		pipelineInfo.pDepthStencilState = &config_info.depthStencilInfo;
		pipelineInfo.pDynamicState = &config_info.dynamicStateInfo;

		pipelineInfo.layout = config_info.pipelineLayout;
		pipelineInfo.renderPass = config_info.renderPass;
		pipelineInfo.subpass = config_info.subpass;

		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create graphics pipeline");
		}
	}
}
