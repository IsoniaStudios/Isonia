// internal
#include "RenderSystems.h"
#include "../../Noise/Noise.h"

// shaders
#include "../../Shaders/Include/Ground/FragShader_frag.h"
#include "../../Shaders/Include/Ground/VertexShader_vert.h"

#include "../../Shaders/Include/Grass/FragShader_frag.h"
#include "../../Shaders/Include/Grass/VertexShader_vert.h"
#include "../../Shaders/Include/Grass/GeomShader_geom.h"

namespace Isonia::Pipeline::RenderSystems
{
	GroundRenderSystem::GroundRenderSystem(Device* device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout)
		: m_device(device)
	{
		createGroundPipelineLayout(global_set_layout);
		createGroundPipeline(render_pass);

		createGrassPipelineLayout(global_set_layout);
		createGrassPipeline(render_pass);

		Noise::ConstantScalarWarpNoise groundWarpNoise{ 0.05f };
		Noise::FractalPerlinNoise groundNoise{ 69, 3, 2.0f, 0.5f, 0.0f };

		const int S_GROUNDS = static_cast<int>(grounds);
		const int S_QUADS = static_cast<int>(Renderable::quads);
		m_grounds = static_cast<Renderable::BuilderXZUniformN*>(operator new[](sizeof(Renderable::BuilderXZUniformN)* grounds_count));
		m_grasses = static_cast<Renderable::BuilderXZUniformNP*>(operator new[](sizeof(Renderable::BuilderXZUniformNP)* grounds_count));
		for (int x = 0; x < grounds; x++)
		{
			for (int z = 0; z < grounds; z++)
			{
				float xOffset = (x - S_GROUNDS / 2l) * S_QUADS * Renderable::quad_size - S_QUADS * Renderable::quad_size * 0.5f;
				float zOffset = (z - S_GROUNDS / 2l) * S_QUADS * Renderable::quad_size - S_QUADS * Renderable::quad_size * 0.5f;
				Renderable::BuilderXZUniformN* ground = new (m_grounds + x * S_GROUNDS + z) Renderable::BuilderXZUniformN(device, &groundWarpNoise, &groundNoise, 7.5f, xOffset, zOffset);
				Renderable::BuilderXZUniformNP* grass = new (m_grasses + x * S_GROUNDS + z) Renderable::BuilderXZUniformNP(device, ground);
			}
		}
	}

	GroundRenderSystem::~GroundRenderSystem()
	{
		delete m_ground_pipeline;
		delete m_grass_pipeline;
		vkDestroyPipelineLayout(m_device->getDevice(), m_ground_pipeline_layout, nullptr);
		vkDestroyPipelineLayout(m_device->getDevice(), m_grass_pipeline_layout, nullptr);

		for (long x = grounds - 1; x >= 0; x--)
		{
			for (long z = grounds - 1; z >= 0; z--)
			{
				m_grounds[x * grounds + z].~BuilderXZUniformN();
				m_grasses[x * grounds + z].~BuilderXZUniformNP();
			}
		}
		operator delete[](m_grounds);
		operator delete[](m_grasses);
	}

	Renderable::BuilderXZUniformN* GroundRenderSystem::mapWorldToGround(const float world_x, const float world_z) const
	{
		unsigned int i_x = static_cast<unsigned int>(world_x / (Renderable::quads * Renderable::quad_size) + grounds / 2);
		unsigned int i_z = static_cast<unsigned int>(world_z / (Renderable::quads * Renderable::quad_size) + grounds / 2);
		return &m_grounds[i_x * grounds + i_z];
	}

	float GroundRenderSystem::mapWorldToHeight(const float world_x, const float world_z) const
	{
		const Renderable::BuilderXZUniformN* ground = mapWorldToGround(world_x, world_z);
		return ground->mapWorldToHeight(world_x, world_z);
	}

	Math::Vector3 GroundRenderSystem::mapWorldToNormal(const float world_x, const float world_z) const
	{
		const Renderable::BuilderXZUniformN* ground = mapWorldToGround(world_x, world_z);
		return ground->mapWorldToNormal(world_x, world_z);
	}

	void GroundRenderSystem::render(State::FrameInfo* frame_info)
	{
		renderGround(frame_info);
		renderGrass(frame_info);
	}

	void GroundRenderSystem::renderGround(State::FrameInfo* frame_info)
	{
		m_ground_pipeline->bind(frame_info->command_buffer);

		vkCmdBindDescriptorSets(
			frame_info->command_buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_ground_pipeline_layout,
			0,
			1,
			&frame_info->global_descriptor_set,
			0,
			nullptr
		);

		for (long x = 0; x < grounds; x++)
		{
			for (long z = 0; z < grounds; z++)
			{
				Renderable::BuilderXZUniformN* ground = &m_grounds[x * grounds + z];

				vkCmdPushConstants(
					frame_info->command_buffer,
					m_ground_pipeline_layout,
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
					0,
					sizeof(Math::Vector2),
					&(ground->m_positional_data)
				);
				ground->bind(frame_info->command_buffer);
				ground->draw(frame_info->command_buffer);
			}
		}
	}

	void GroundRenderSystem::renderGrass(State::FrameInfo* frame_info)
	{
		m_grass_pipeline->bind(frame_info->command_buffer);

		vkCmdBindDescriptorSets(
			frame_info->command_buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_grass_pipeline_layout,
			0,
			1,
			&frame_info->global_descriptor_set,
			0,
			nullptr
		);

		for (long x = 0; x < grounds; x++)
		{
			for (long z = 0; z < grounds; z++)
			{
				Renderable::BuilderXZUniformNP* grass = &m_grasses[x * grounds + z];

				grass->bind(frame_info->command_buffer);
				grass->draw(frame_info->command_buffer);
			}
		}
	}

	void GroundRenderSystem::createGroundPipelineLayout(VkDescriptorSetLayout global_set_layout)
	{
		VkPushConstantRange push_constant_range{};
		push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		push_constant_range.offset = 0;
		push_constant_range.size = sizeof(Math::Vector2);

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
		if (vkCreatePipelineLayout(m_device->getDevice(), &pipeline_layout_info, nullptr, &m_ground_pipeline_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void GroundRenderSystem::createGroundPipeline(VkRenderPass render_pass)
	{
		assert(m_ground_pipeline_layout != nullptr && "Cannot create pipeline before a pipeline layout is instantiated");

		PipelineConfigInfo pipeline_config{};
		Pipeline::pixelPipelineTriangleStripNormalConfigInfo(&pipeline_config);
		pipeline_config.renderPass = render_pass;
		pipeline_config.pipelineLayout = m_ground_pipeline_layout;
		m_ground_pipeline = (new Pipeline::Builder(m_device))
			->addShaderModule(
				VK_SHADER_STAGE_VERTEX_BIT,
				Shaders::Ground::VERTEXSHADER_VERT,
				sizeof(Shaders::Ground::VERTEXSHADER_VERT) / sizeof(unsigned char)
			)->addShaderModule(
				VK_SHADER_STAGE_FRAGMENT_BIT,
				Shaders::Ground::FRAGSHADER_FRAG,
				sizeof(Shaders::Ground::FRAGSHADER_FRAG) / sizeof(unsigned char)
			)->createGraphicsPipeline(&pipeline_config);
	}

	void GroundRenderSystem::createGrassPipelineLayout(VkDescriptorSetLayout global_set_layout)
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
		if (vkCreatePipelineLayout(m_device->getDevice(), &pipeline_layout_info, nullptr, &m_grass_pipeline_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void GroundRenderSystem::createGrassPipeline(VkRenderPass render_pass)
	{
		assert(m_grass_pipeline_layout != nullptr && "Cannot create pipeline before a pipeline layout is instantiated");

		PipelineConfigInfo pipeline_config{};
		pixelPipelinePointListConfigInfo(&pipeline_config);
		pipeline_config.renderPass = render_pass;
		pipeline_config.pipelineLayout = m_grass_pipeline_layout;
		m_grass_pipeline = (new Pipeline::Builder(m_device))
			->addShaderModule(
				VK_SHADER_STAGE_GEOMETRY_BIT,
				Shaders::Grass::GEOMSHADER_GEOM,
				sizeof(Shaders::Grass::GEOMSHADER_GEOM) / sizeof(unsigned char)
			)->addShaderModule(
				VK_SHADER_STAGE_VERTEX_BIT,
				Shaders::Grass::VERTEXSHADER_VERT,
				sizeof(Shaders::Grass::VERTEXSHADER_VERT) / sizeof(unsigned char)
			)->addShaderModule(
				VK_SHADER_STAGE_FRAGMENT_BIT,
				Shaders::Grass::FRAGSHADER_FRAG,
				sizeof(Shaders::Grass::FRAGSHADER_FRAG) / sizeof(unsigned char)
			)->createGraphicsPipeline(&pipeline_config);
	}

	void GroundRenderSystem::pixelPipelinePointListConfigInfo(PipelineConfigInfo* config_info)
	{
		Pipeline::pixelPipelineConfigInfo(config_info);

		config_info->input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

		config_info->binding_descriptions = Renderable::VertexXZUniformNP::getBindingDescriptions();
		config_info->attribute_descriptions = Renderable::VertexXZUniformNP::getAttributeDescriptions();
	}
}
