// internal
#include "RenderSystems.h"

// shaders
#include "../../Shaders/Include/Billboard/FragShader_frag.h"
#include "../../Shaders/Include/Billboard/VertexShader_vert.h"
#include "../../Shaders/Include/Billboard/GeomShader_geom.h"

namespace Isonia::Pipeline::RenderSystems
{
	DebuggerRenderSystem::DebuggerRenderSystem(Device* device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout)
		: m_device(device)
	{
		createPipelineLayout(global_set_layout);
		createPipeline(render_pass);

		m_debugger = new Renderable::BuilderPosition(m_device);
	}

	DebuggerRenderSystem::~DebuggerRenderSystem()
	{
		delete m_pipeline;
		vkDestroyPipelineLayout(m_device->getDevice(), m_pipeline_layout, nullptr);

		delete m_debugger;
	}

	void DebuggerRenderSystem::render(State::FrameInfo* frame_info)
	{
		m_pipeline->bind(frame_info->command_buffer);

		vkCmdBindDescriptorSets(
			frame_info->command_buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipeline_layout,
			0,
			1,
			&frame_info->global_descriptor_set,
			0,
			nullptr
		);

		m_debugger->bind(frame_info->command_buffer);
		m_debugger->draw(frame_info->command_buffer);
	}

	void DebuggerRenderSystem::createPipelineLayout(VkDescriptorSetLayout global_set_layout)
	{
		const constexpr uint32_t descriptor_set_layouts_length = 1;
		const VkDescriptorSetLayout descriptor_set_layouts[descriptor_set_layouts_length]{
			global_set_layout
		};

		VkPipelineLayoutCreateInfo pipeline_layout_info{};
		pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_info.setLayoutCount = descriptor_set_layouts_length;
		pipeline_layout_info.pSetLayouts = descriptor_set_layouts;
		pipeline_layout_info.pushConstantRangeCount = 0;
		pipeline_layout_info.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(m_device->getDevice(), &pipeline_layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void DebuggerRenderSystem::createPipeline(VkRenderPass render_pass)
	{
		assert(m_pipeline_layout != nullptr && "Cannot create pipeline before a pipeline layout is instantiated");

		PipelineConfigInfo pipeline_config{};
		pixelPipelinePointListConfigInfo(&pipeline_config);
		pipeline_config.renderPass = render_pass;
		pipeline_config.pipelineLayout = m_pipeline_layout;
		m_pipeline = (new Pipeline::Builder(m_device))
			->addShaderModule(
				VK_SHADER_STAGE_GEOMETRY_BIT,
				Shaders::Billboard::GEOMSHADER_GEOM,
				sizeof(Shaders::Billboard::GEOMSHADER_GEOM) / sizeof(unsigned char)
			)->addShaderModule(
				VK_SHADER_STAGE_VERTEX_BIT,
				Shaders::Billboard::VERTEXSHADER_VERT,
				sizeof(Shaders::Billboard::VERTEXSHADER_VERT) / sizeof(unsigned char)
			)->addShaderModule(
				VK_SHADER_STAGE_FRAGMENT_BIT,
				Shaders::Billboard::FRAGSHADER_FRAG,
				sizeof(Shaders::Billboard::FRAGSHADER_FRAG) / sizeof(unsigned char)
			)->createGraphicsPipeline(&pipeline_config);
	}

	constexpr void DebuggerRenderSystem::pixelPipelinePointListConfigInfo(PipelineConfigInfo* pipeline_config)
	{
		Pipeline::pixelPipelineConfigInfo(pipeline_config);

		pipeline_config->input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

		pipeline_config->binding_descriptions = Renderable::VertexPosition::getBindingDescriptions();
		pipeline_config->attribute_descriptions = Renderable::VertexPosition::getAttributeDescriptions();
	}
}
