// internal
#include "RenderSystems.h"

// shaders
#include "../../Shaders/Include/Water/FragShader_frag.h"
#include "../../Shaders/Include/Water/VertexShader_vert.h"

// external
#include <stdexcept>

namespace Isonia::Pipeline::RenderSystems
{
	WaterRenderSystem::WaterRenderSystem(Device* device, const VkRenderPass render_pass, const VkDescriptorSetLayout global_set_layout)
		: m_device(device)
	{
		createpipelineLayout(global_set_layout);
		createpipeline(render_pass);

		m_water = new Renderable::BuilderXZUniform(m_device, 2u, 100.0f);
	}

	WaterRenderSystem::~WaterRenderSystem()
	{
		delete m_pipeline;
		vkDestroyPipelineLayout(m_device->getDevice(), m_pipeline_layout, nullptr);

		delete m_water;
	}

	void WaterRenderSystem::render(const State::FrameInfo* frame_info, const Camera* camera)
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

		const float offset_to_center = -64.0f;
		const Math::Vector3 camera_position = camera->getPositionVector();
		const Math::Vector3 camera_forward = camera->getForwardVector();
		const Math::Vector3 plane_position = Math::Vector3{ 0.0f, -5.0f, 0.0f };
		const Math::Vector3 plane_normal = Math::Vector3{ 0.0f, -1.0f, 0.0f };
		float intersection_distance;
		const bool intersects = Math::intersectRayPlane(
			&camera_position,
			&camera_forward,
			&plane_position,
			&plane_normal,
			&intersection_distance
		);
		Math::Vector3 intersection_point_local = Math::vec3Mul(&camera_forward, intersection_distance);
		Math::Vector3 intersection_point = Math::vec3Add(&camera_position, &intersection_point_local);
		m_water->m_position = {
			offset_to_center + intersection_point.x,
			0.25f,
			offset_to_center + intersection_point.z
		};

		vkCmdPushConstants(
			frame_info->command_buffer,
			m_pipeline_layout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(Math::Vector3),
			&(m_water->m_position)
		);
		m_water->bind(frame_info->command_buffer);
		m_water->draw(frame_info->command_buffer);
	}

	void WaterRenderSystem::createpipelineLayout(const VkDescriptorSetLayout global_set_layout)
	{
		VkPushConstantRange push_constant_range{};
		push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		push_constant_range.offset = 0;
		push_constant_range.size = sizeof(Math::Vector3);

		const constexpr unsigned int descriptor_set_layouts_length = 1;
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

	void WaterRenderSystem::createpipeline(const VkRenderPass render_pass)
	{
		assert(m_pipeline_layout != nullptr && "Cannot create pipeline before a pipeline layout is instantiated");

		PipelineConfigInfo pipeline_config{};
		Pipeline::pixelPipelineTriangleStripConfigInfo(&pipeline_config);
		Pipeline::makeTransparentConfigInfo(&pipeline_config);
		pipeline_config.renderPass = render_pass;
		pipeline_config.pipelineLayout = m_pipeline_layout;
		m_pipeline = (new Pipeline(m_device, 2u))
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
