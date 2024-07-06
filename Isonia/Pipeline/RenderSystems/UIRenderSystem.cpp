// internal
#include "RenderSystems.h"

// shaders
#include "../../Shaders/Include/UI/FragShader_frag.h"
#include "../../Shaders/Include/UI/VertexShader_vert.h"

// external
#include <stdexcept>

namespace Isonia::Pipeline::RenderSystems
{
	UIRenderSystem::UIRenderSystem(Device* device, const VkRenderPass render_pass, const VkDescriptorSetLayout global_set_layout, const unsigned int max_text_length)
		: m_device(device), m_ui(nullptr)
	{
		createPipelineLayout(global_set_layout);
		createPipeline(render_pass);

		m_ui = new Renderable::BuilderUI(m_device, max_text_length);
	}

	UIRenderSystem::~UIRenderSystem()
	{
		delete m_pipeline;
		vkDestroyPipelineLayout(m_device->getDevice(), m_pipeline_layout, nullptr);

		delete m_ui;
	}

	void UIRenderSystem::update(const char* text)
	{
		m_ui->update(text);
	}

	void UIRenderSystem::render(const State::FrameInfo* frame_info, const Camera* camera)
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

		m_ui->bind(frame_info->command_buffer);
		m_ui->draw(frame_info->command_buffer);
	}

	void UIRenderSystem::createPipelineLayout(const VkDescriptorSetLayout global_set_layout)
	{
		const constexpr unsigned int descriptor_set_layouts_length = 1;
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

	void UIRenderSystem::createPipeline(const VkRenderPass render_pass)
	{
		assert(m_pipeline_layout != nullptr && "Cannot create pipeline before a pipeline layout is instantiated");

		PipelineConfigInfo pipeline_config{};
		pipelineConfigInfo(&pipeline_config);
		pipeline_config.renderPass = render_pass;
		pipeline_config.pipelineLayout = m_pipeline_layout;
		m_pipeline = (new Pipeline(m_device, 2u))
			->addShaderModule(
				VK_SHADER_STAGE_VERTEX_BIT,
				Shaders::UI::VERTEXSHADER_VERT,
				sizeof(Shaders::UI::VERTEXSHADER_VERT) / sizeof(unsigned char)
			)->addShaderModule(
				VK_SHADER_STAGE_FRAGMENT_BIT,
				Shaders::UI::FRAGSHADER_FRAG,
				sizeof(Shaders::UI::FRAGSHADER_FRAG) / sizeof(unsigned char)
			)->createGraphicsPipeline(&pipeline_config);
	}

	void UIRenderSystem::pipelineConfigInfo(PipelineConfigInfo* pipeline_config)
	{
		Pipeline::pixelPipelineConfigInfo(pipeline_config);

		pipeline_config->binding_descriptions = Renderable::VertexUI::getBindingDescriptions();
		pipeline_config->binding_descriptions_count = Renderable::VertexUI::getBindingDescriptionsCount();
		pipeline_config->attribute_descriptions = Renderable::VertexUI::getAttributeDescriptions();
		pipeline_config->attribute_descriptions_count = Renderable::VertexUI::getAttributeDescriptionsCount();
	}
}
