// internal
#include "RenderSystems.h"
#include "../../Noise/Noise.h"

// shaders
#include "../../Shaders/Include/Ground/FragShader_frag.h"
#include "../../Shaders/Include/Ground/VertexShader_vert.h"

#include "../../Shaders/Include/Grass/FragShader_frag.h"
#include "../../Shaders/Include/Grass/VertexShader_vert.h"
#include "../../Shaders/Include/Grass/GeomShader_geom.h"

// external
#include <new>
#include <stdexcept>

namespace Isonia::Pipeline::RenderSystems
{
	GroundRenderSystem::GroundRenderSystem(Device* device, const VkRenderPass render_pass, const VkDescriptorSetLayout global_set_layout)
		: m_device(device)
	{
		createGroundPipelineLayout(global_set_layout);
		createGroundPipeline(render_pass);

		createGrassPipelineLayout(global_set_layout);
		createGrassPipeline(render_pass);
	}

	GroundRenderSystem::~GroundRenderSystem()
	{
		delete m_ground_pipeline;
		delete m_grass_pipeline;
		vkDestroyPipelineLayout(m_device->getDevice(), m_ground_pipeline_layout, nullptr);
		vkDestroyPipelineLayout(m_device->getDevice(), m_grass_pipeline_layout, nullptr);

		for (unsigned int x = 0; x < grounds; x++)
		{
			for (unsigned int z = 0; z < grounds; z++)
			{
				if (m_grounds[x][z] != nullptr)
				{
					delete m_grounds[x][z];
				}
				if (m_grasses[x][z] != nullptr)
				{
					delete m_grasses[x][z];
				}
			}
		}
	}

	unsigned int GroundRenderSystem::mapWorldXToIndex(const float world_x) const
	{
		const float ground_width = Renderable::quads * Renderable::quad_size;
		const float local_x = world_x + ground_width * grounds / 2u;
		return static_cast<unsigned int>(local_x / ground_width);
	}
	float GroundRenderSystem::mapIndexToWorldX(const unsigned int index) const
	{
		const float ground_width = Renderable::quads * Renderable::quad_size;
		const float local_x = index * ground_width;
		return local_x - ground_width * grounds / 2u + Renderable::quads * Renderable::quad_size * 0.5f;
	}
	unsigned int GroundRenderSystem::mapWorldZToIndex(const float world_z) const
	{
		const float ground_height = Renderable::quads * Renderable::quad_size;
		const float local_z = world_z + ground_height * grounds / 2u;
		return static_cast<unsigned int>(local_z / ground_height);
	}
	float GroundRenderSystem::mapIndexToWorldZ(const unsigned int index) const
	{
		const float ground_height = Renderable::quads * Renderable::quad_size;
		const float local_z = index * ground_height;
		return local_z - ground_height * grounds / 2u + Renderable::quads * Renderable::quad_size * 0.5f;
	}
	Renderable::BuilderXZUniformN* GroundRenderSystem::mapWorldToGround(const float world_x, const float world_z) const
	{
		unsigned int index_x = mapWorldXToIndex(world_x);
		unsigned int index_z = mapWorldZToIndex(world_z);
		return m_grounds[index_x][index_z];
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

	void GroundRenderSystem::frustumCull(const Camera* camera)
	{
		for (int index_x = 0; index_x < grounds; index_x++)
		{
			for (int index_z = 0; index_z < grounds; index_z++)
			{
				const Math::BoundingPlane bounding_plane{
					{mapIndexToWorldX(index_x), 0.0f, mapIndexToWorldX(index_z)},
					{Renderable::quads * Renderable::quad_size, Renderable::quads * Renderable::quad_size}
				};

				if (camera->inFustrum(bounding_plane))
				{
					Renderable::BuilderXZUniformN** ground = &m_grounds[index_x][index_z];
					if (*ground == nullptr)
					{
						const int s_grounds = static_cast<int>(grounds);
						const float w_ground = Renderable::quads * Renderable::quad_size;

						const float offset = grounds % 2u == 0u ? 0.0f : w_ground * 0.5f;
						const float x_offset = (index_x - s_grounds / 2) * w_ground - offset;
						const float z_offset = (index_z - s_grounds / 2) * w_ground - offset;
						*ground = new Renderable::BuilderXZUniformN(m_device, &m_ground_warp_noise, &m_ground_noise, 7.5f, x_offset, z_offset);
					}
					else
					{
						(*ground)->m_culled = false;
					}
					Renderable::BuilderXZUniformNP** grass = &m_grasses[index_x][index_z];
					if (*grass == nullptr)
					{
						*grass = new Renderable::BuilderXZUniformNP(m_device, *ground);
					}
					else
					{
						(*grass)->m_culled = false;
					}
				}
				else
				{
					if (m_grounds[index_x][index_z] != nullptr)
					{
						m_grounds[index_x][index_z]->m_culled = true;
					}
					if (m_grasses[index_x][index_z] != nullptr)
					{
						m_grasses[index_x][index_z]->m_culled = true;
					}
				}
			}
		}
	}

	void GroundRenderSystem::render(const State::FrameInfo* frame_info, const Camera* camera)
	{
		frustumCull(camera);
		renderGround(frame_info);
		renderGrass(frame_info);
	}

	void GroundRenderSystem::renderGround(const State::FrameInfo* frame_info)
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

		for (unsigned int x = 0; x < grounds; x++)
		{
			for (unsigned int z = 0; z < grounds; z++)
			{
				Renderable::BuilderXZUniformN* ground = m_grounds[x][z];
				if (ground == nullptr || ground->m_culled)
				{
					continue;
				}

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

	void GroundRenderSystem::renderGrass(const State::FrameInfo* frame_info)
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

		for (unsigned int x = 0; x < grounds; x++)
		{
			for (unsigned int z = 0; z < grounds; z++)
			{
				Renderable::BuilderXZUniformNP* grass = m_grasses[x][z];
				if (grass == nullptr || grass->m_culled)
				{
					continue;
				}

				grass->bind(frame_info->command_buffer);
				grass->draw(frame_info->command_buffer);
			}
		}
	}

	void GroundRenderSystem::createGroundPipelineLayout(const VkDescriptorSetLayout global_set_layout)
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

	void GroundRenderSystem::createGroundPipeline(const VkRenderPass render_pass)
	{
		assert(m_ground_pipeline_layout != nullptr && "Cannot create pipeline before a pipeline layout is instantiated");

		PipelineConfigInfo pipeline_config{};
		Pipeline::pixelPipelineTriangleStripNormalConfigInfo(&pipeline_config);
		pipeline_config.renderPass = render_pass;
		pipeline_config.pipelineLayout = m_ground_pipeline_layout;
		m_ground_pipeline = (new Pipeline(m_device, 2u))
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

	void GroundRenderSystem::createGrassPipelineLayout(const VkDescriptorSetLayout global_set_layout)
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

	void GroundRenderSystem::createGrassPipeline(const VkRenderPass render_pass)
	{
		assert(m_grass_pipeline_layout != nullptr && "Cannot create pipeline before a pipeline layout is instantiated");

		PipelineConfigInfo pipeline_config{};
		pixelPipelinePointListConfigInfo(&pipeline_config);
		pipeline_config.renderPass = render_pass;
		pipeline_config.pipelineLayout = m_grass_pipeline_layout;
		m_grass_pipeline = (new Pipeline(m_device, 3u))
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
		config_info->binding_descriptions_count = Renderable::VertexXZUniformNP::getBindingDescriptionsCount();
		config_info->attribute_descriptions = Renderable::VertexXZUniformNP::getAttributeDescriptions();
		config_info->attribute_descriptions_count = Renderable::VertexXZUniformNP::getAttributeDescriptionsCount();
	}
}
