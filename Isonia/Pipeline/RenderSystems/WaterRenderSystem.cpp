// internal
#include "RenderSystems.h"

// shaders
#include "../../Shaders/Include/Water/FragShader_frag.h"
#include "../../Shaders/Include/Water/VertexShader_vert.h"

namespace Isonia::Pipeline::RenderSystems
{
	WaterRenderSystem::WaterRenderSystem(Device* device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout)
		: m_device(device)
	{
		createpipelineLayout(global_set_layout);
		createpipeline(render_pass);

		m_water = new Renderable::XZUniform::Builder(m_device);
	}

	WaterRenderSystem::~WaterRenderSystem()
	{
		delete m_pipeline;
		vkDestroyPipelineLayout(m_device->getDevice(), m_pipeline_layout, nullptr);

		delete m_water;
	}

	void WaterRenderSystem::render(State::FrameInfo* frame_info, Camera* camera)
	{
		m_pipeline->bind(frame_info->commandBuffer);

		vkCmdBindDescriptorSets(
			frame_info->commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipeline_layout,
			0,
			1,
			&frame_info->globalDescriptorSet,
			0,
			nullptr
		);

		constexpr const float offsetToCenter = -(Renderable::XZUniform::QUADS * Renderable::XZUniform::QUAD_SIZE * 0.5f);
		const Math::Vector3 cameraPosition = camera->getPositionVector();
		const Math::Vector3 cameraForward = camera->getForwardVector();
		float intersectionDistance;
		const bool intersects = Math::intersectRayPlane(
			&cameraPosition,
			&cameraForward,
			&Math::Vector3{ 0.0f, -5.0f, 0.0f },
			&Math::Vector3{ 0.0f, -1.0f, 0.0f },
			&intersectionDistance
		);
		Math::Vector3 intersectionPoint = Math::vec3Add(&cameraPosition, &Math::vec3Mul(&cameraForward, intersectionDistance));
		m_water->position = {
			offsetToCenter + intersectionPoint.x,
			-5,
			offsetToCenter + intersectionPoint.z
		};

		vkCmdPushConstants(
			frame_info->commandBuffer,
			m_pipeline_layout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(Math::Vector3),
			&(m_water->position)
		);
		m_water->bind(frame_info->commandBuffer);
		m_water->draw(frame_info->commandBuffer);
	}

	void WaterRenderSystem::createpipelineLayout(VkDescriptorSetLayout global_set_layout)
	{
		VkPushConstantRange push_constant_range{};
		push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		push_constant_range.offset = 0;
		push_constant_range.size = sizeof(Math::Vector3);

		const constexpr uint32_t descriptor_set_layouts_length = 1;
		const VkDescriptorSetLayout descriptor_set_layouts[descriptor_set_layouts_length]{
			global_set_layout
		};

		VkPipelineLayoutCreateInfo pipeline_layout_info{};
		pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_info.setLayoutCount = descriptor_set_layouts_length;
		pipeline_layout_info.pSetLayouts = descriptor_set_layouts;
		pipeline_layout_info.pushConstantRangeCount = 1;
		pipeline_layout_info.pPushConstantRanges = &push_constant_range;
		if (vkCreatePipelineLayout(m_device->getDevice(), &pipeline_layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void WaterRenderSystem::createpipeline(VkRenderPass render_pass)
	{
		assert(m_pipeline_layout != nullptr && "Cannot create pipeline before a pipeline layout is instantiated");

		PipelineConfigInfo pipeline_config{};
		Pipeline::pixelPipelineTriangleStripConfigInfo(&pipeline_config);
		Pipeline::makeTransparentConfigInfo(&pipeline_config);
		pipeline_config.renderPass = render_pass;
		pipeline_config.pipelineLayout = m_pipeline_layout;
		m_pipeline = (new Pipeline::Builder(m_device))
			->addShaderModule(
				VK_SHADER_STAGE_VERTEX_BIT,
				Shaders::Water::VERTEXSHADER_VERT,
				sizeof(Shaders::Water::VERTEXSHADER_VERT) / sizeof(unsigned char)
			)->addShaderModule(
				VK_SHADER_STAGE_FRAGMENT_BIT,
				Shaders::Water::FRAGSHADER_FRAG,
				sizeof(Shaders::Water::FRAGSHADER_FRAG) / sizeof(unsigned char)
			)->createGraphicsPipeline(&pipeline_config);
	}
}
