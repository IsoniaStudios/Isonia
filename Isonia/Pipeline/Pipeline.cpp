// internal
#include "Pipeline.h"
#include "../Renderable/Renderable.h"

// external
#include <stdexcept>

namespace Isonia::Pipeline
{
	Pipeline::Builder::Builder(Device* device, unsigned int m_shader_stages_count)
		: m_device(device), m_shader_stages_count(m_shader_stages_count), m_shader_stages(new VkPipelineShaderStageCreateInfo[m_shader_stages_count])
	{
	}

	Pipeline::Builder* Pipeline::Builder::addShaderModule(VkShaderStageFlagBits stage, const unsigned char* const code, const unsigned int size)
	{
		VkShaderModule shader_module = createShaderModule(code, size);		
		m_shader_stages[m_shader_stages_index++] = VkPipelineShaderStageCreateInfo{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			stage,
			shader_module,
			"main",
			nullptr
		};

		return this;
	}

	Pipeline* Pipeline::Builder::createGraphicsPipeline(const PipelineConfigInfo* config_info) const
	{
		return new Pipeline(m_device, m_shader_stages, m_shader_stages_count, config_info);
	}

	VkShaderModule Pipeline::Builder::createShaderModule(const unsigned char* const code, const unsigned int size)
	{
		VkShaderModuleCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = size;
		create_info.pCode = reinterpret_cast<const unsigned int*>(code);

		VkShaderModule shader_module{};
		if (vkCreateShaderModule(m_device->getDevice(), &create_info, nullptr, &shader_module) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module");
		}
		return shader_module;
	}

	Pipeline::Pipeline(Device* device, VkPipelineShaderStageCreateInfo* shader_stages, const unsigned int shader_stages_count, const PipelineConfigInfo* config_info)
		: m_device(device)
	{
		createGraphicsPipeline(shader_stages, shader_stages_count, config_info);
	}

	Pipeline::~Pipeline()
	{
		for (unsigned int i = 0; i < m_shader_stages_count; i++)
		{
			vkDestroyShaderModule(m_device->getDevice(), m_shader_stages[i].module, nullptr);
		}
		vkDestroyPipeline(m_device->getDevice(), m_graphics_pipeline, nullptr);
	}

	VkShaderStageFlags Pipeline::getStageFlags() const
	{
		return m_stage_flags;
	}

	void Pipeline::bind(VkCommandBuffer command_buffer)
	{
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);
	}

	void Pipeline::defaultPipelineConfigInfo(PipelineConfigInfo* config_info)
	{
		config_info->input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		config_info->input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		config_info->input_assembly_info.primitiveRestartEnable = VK_FALSE;

		config_info->viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		config_info->viewport_info.viewportCount = 1;
		config_info->viewport_info.pViewports = nullptr;
		config_info->viewport_info.scissorCount = 1;
		config_info->viewport_info.pScissors = nullptr;

		config_info->rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		config_info->rasterization_info.depthClampEnable = VK_FALSE;
		config_info->rasterization_info.rasterizerDiscardEnable = VK_FALSE;
		config_info->rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
		config_info->rasterization_info.lineWidth = 1.0f;
		config_info->rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT;
		config_info->rasterization_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		config_info->rasterization_info.depthBiasEnable = VK_FALSE;
		config_info->rasterization_info.depthBiasConstantFactor = 0.0f;  // Optional
		config_info->rasterization_info.depthBiasClamp = 0.0f;           // Optional
		config_info->rasterization_info.depthBiasSlopeFactor = 0.0f;     // Optional

		config_info->multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		config_info->multisample_info.sampleShadingEnable = VK_FALSE;
		config_info->multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		config_info->multisample_info.minSampleShading = 1.0f;           // Optional
		config_info->multisample_info.pSampleMask = nullptr;             // Optional
		config_info->multisample_info.alphaToCoverageEnable = VK_FALSE;  // Optional
		config_info->multisample_info.alphaToOneEnable = VK_FALSE;       // Optional

		config_info->color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		config_info->color_blend_attachment.blendEnable = VK_FALSE;
		config_info->color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		config_info->color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		config_info->color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		config_info->color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		config_info->color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		config_info->color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

		config_info->color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		config_info->color_blend_info.logicOpEnable = VK_FALSE;
		config_info->color_blend_info.logicOp = VK_LOGIC_OP_COPY;  // Optional
		config_info->color_blend_info.attachmentCount = 1;
		config_info->color_blend_info.pAttachments = &config_info->color_blend_attachment;
		config_info->color_blend_info.blendConstants[0] = 0.0f;  // Optional
		config_info->color_blend_info.blendConstants[1] = 0.0f;  // Optional
		config_info->color_blend_info.blendConstants[2] = 0.0f;  // Optional
		config_info->color_blend_info.blendConstants[3] = 0.0f;  // Optional

		config_info->depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		config_info->depth_stencil_info.depthTestEnable = VK_TRUE;
		config_info->depth_stencil_info.depthWriteEnable = VK_TRUE;
		config_info->depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
		config_info->depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
		config_info->depth_stencil_info.minDepthBounds = 0.0f;  // Optional
		config_info->depth_stencil_info.maxDepthBounds = 1.0f;  // Optional
		config_info->depth_stencil_info.stencilTestEnable = VK_FALSE;
		config_info->depth_stencil_info.front = {};  // Optional
		config_info->depth_stencil_info.back = {};   // Optional

		config_info->dynamic_state_enables = new VkDynamicState[]{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		config_info->dynamic_state_enables_count = 2u;
		config_info->dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		config_info->dynamic_state_info.pDynamicStates = config_info->dynamic_state_enables;
		config_info->dynamic_state_info.dynamicStateCount = config_info->dynamic_state_enables_count;
		config_info->dynamic_state_info.flags = 0;

		config_info->binding_descriptions = Renderable::VertexComplete::getBindingDescriptions();
		config_info->binding_descriptions_count = Renderable::VertexComplete::getBindingDescriptionsCount();
		config_info->attribute_descriptions = Renderable::VertexComplete::getAttributeDescriptions();
		config_info->attribute_descriptions_count = Renderable::VertexComplete::getAttributeDescriptionsCount();
	}

	void Pipeline::makePixelPerfectConfigInfo(PipelineConfigInfo* config_info)
	{
		config_info->rasterization_info.lineWidth = 1.0f;

		config_info->multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		config_info->multisample_info.sampleShadingEnable = VK_FALSE;

		config_info->color_blend_attachment.blendEnable = VK_FALSE;

		config_info->depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
		config_info->depth_stencil_info.stencilTestEnable = VK_FALSE;
	}

	void Pipeline::makeTransparentConfigInfo(PipelineConfigInfo* config_info)
	{
		config_info->color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		config_info->color_blend_attachment.blendEnable = VK_TRUE;
		config_info->color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		config_info->color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		config_info->color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
		config_info->color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		config_info->color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		config_info->color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	}

	void Pipeline::makeTriangleStripConfigInfo(PipelineConfigInfo* config_info)
	{
		config_info->input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		config_info->rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	}

	void Pipeline::pixelPipelineTriangleStripConfigInfo(PipelineConfigInfo* config_info)
	{
		defaultPipelineConfigInfo(config_info);
		makePixelPerfectConfigInfo(config_info);
		makeTriangleStripConfigInfo(config_info);

		config_info->binding_descriptions = Renderable::VertexXZUniform::getBindingDescriptions();
		config_info->binding_descriptions_count = Renderable::VertexXZUniform::getBindingDescriptionsCount();
		config_info->attribute_descriptions = Renderable::VertexXZUniform::getAttributeDescriptions();
		config_info->attribute_descriptions_count = Renderable::VertexXZUniform::getAttributeDescriptionsCount();
	}

	void Pipeline::pixelPipelineTriangleStripNormalConfigInfo(PipelineConfigInfo* config_info)
	{
		defaultPipelineConfigInfo(config_info);
		makePixelPerfectConfigInfo(config_info);
		makeTriangleStripConfigInfo(config_info);

		config_info->binding_descriptions = Renderable::VertexXZUniformN::getBindingDescriptions();
		config_info->binding_descriptions_count = Renderable::VertexXZUniformN::getBindingDescriptionsCount();
		config_info->attribute_descriptions = Renderable::VertexXZUniformN::getAttributeDescriptions();
		config_info->attribute_descriptions_count = Renderable::VertexXZUniformN::getAttributeDescriptionsCount();
	}

	void Pipeline::pixelPipelineConfigInfo(PipelineConfigInfo* config_info)
	{
		defaultPipelineConfigInfo(config_info);
		makePixelPerfectConfigInfo(config_info);
	}

	void Pipeline::createGraphicsPipeline(VkPipelineShaderStageCreateInfo* shader_stages, const unsigned int shader_stages_count, const PipelineConfigInfo* config_info)
	{
		assert(config_info->pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
		assert(config_info->renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo");

		VkPipelineVertexInputStateCreateInfo vertex_input_info{};
		vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_info.vertexAttributeDescriptionCount = config_info->attribute_descriptions_count;
		vertex_input_info.vertexBindingDescriptionCount = config_info->binding_descriptions_count;
		vertex_input_info.pVertexAttributeDescriptions = config_info->attribute_descriptions;
		vertex_input_info.pVertexBindingDescriptions = config_info->binding_descriptions;

		m_shader_stages = shader_stages;
		m_shader_stages_count = shader_stages_count;
		for (unsigned int i = 0; i < shader_stages_count; i++)
		{
			m_stage_flags |= shader_stages[i].stage;
		}

		VkGraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.stageCount = shader_stages_count;
		pipeline_info.pStages = shader_stages;
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &config_info->input_assembly_info;
		pipeline_info.pViewportState = &config_info->viewport_info;
		pipeline_info.pRasterizationState = &config_info->rasterization_info;
		pipeline_info.pMultisampleState = &config_info->multisample_info;
		pipeline_info.pColorBlendState = &config_info->color_blend_info;
		pipeline_info.pDepthStencilState = &config_info->depth_stencil_info;
		pipeline_info.pDynamicState = &config_info->dynamic_state_info;

		pipeline_info.layout = config_info->pipelineLayout;
		pipeline_info.renderPass = config_info->renderPass;
		pipeline_info.subpass = config_info->subpass;

		pipeline_info.basePipelineIndex = -1;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(m_device->getDevice(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_graphics_pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create graphics pipeline");
		}
	}
}
