#pragma once

// internal
#include "../Device.h"
#include "../Pipeline.h"
#include "../Descriptors/Descriptors.h"

#include "../../State/FrameInfo.h"

#include "../../ECS/System.h"
#include "../../ECS/Coordinator.h"

#include "../../Components/Mesh.h"
#include "../../Components/Camera.h"
#include "../../Components/Transform.h"

#include "../../Renderable/XZUniformN/Builder.h"
#include "../../Renderable/XZUniformN/Grass/Builder.h"
#include "../../Renderable/Color/Color.h"

#include "../../Noise/FractalPerlinNoise.h"

// shaders
#include "../../Shaders/Include/Ground/FragShader_frag.h"
#include "../../Shaders/Include/Ground/VertexShader_vert.h"

#include "../../Shaders/Include/Grass/FragShader_frag.h"
#include "../../Shaders/Include/Grass/VertexShader_vert.h"
#include "../../Shaders/Include/Grass/GeomShader_geom.h"

// std
#include <memory>
#include <vector>
#include <array>
#include <cassert>
#include <stdexcept>
#include <numeric>

extern Isonia::ECS::Coordinator* gCoordinator;

namespace Isonia::Pipeline::Systems
{
	static constexpr const uint32_t GROUNDS = 3;
	static constexpr const uint32_t GROUNDS_COUNT = GROUNDS * GROUNDS;

	class GroundRenderSystem
	{
	public:
		GroundRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device)
		{
			CreateGroundPipelineLayout(globalSetLayout);
			CreateGroundPipeline(renderPass);

			CreateGrassPipelineLayout(globalSetLayout);
			CreateGrassPipeline(renderPass);

			Noise::ConstantScalarWarpNoise groundWarpNoise{ 0.05f };
			Noise::FractalPerlinNoise groundNoise{ 69, 3, 2.0f, 0.5f, 0.0f };

			const int32_t S_GROUNDS = static_cast<int32_t>(GROUNDS);
			const int32_t S_QUADS = static_cast<int32_t>(Renderable::XZUniformN::QUADS);
			grounds = static_cast<Renderable::XZUniformN::Builder*>(operator new[](sizeof(Renderable::XZUniformN::Builder) * GROUNDS_COUNT));
			grasses = static_cast<Renderable::XZUniformN::Grass::Builder*>(operator new[](sizeof(Renderable::XZUniformN::Grass::Builder) * GROUNDS_COUNT));
			for (int32_t x = 0; x < GROUNDS; x++)
			{
				for (int32_t z = 0; z < GROUNDS; z++)
				{
					float xOffset = (x - S_GROUNDS / 2l) * S_QUADS * Renderable::XZUniformN::QUAD_SIZE - S_QUADS * Renderable::XZUniformN::QUAD_SIZE * 0.5f;
					float zOffset = (z - S_GROUNDS / 2l) * S_QUADS * Renderable::XZUniformN::QUAD_SIZE - S_QUADS * Renderable::XZUniformN::QUAD_SIZE * 0.5f;
					auto ground = new (grounds + x * S_GROUNDS + z) Renderable::XZUniformN::Builder(device, groundWarpNoise, groundNoise, 7.5f, xOffset, zOffset);
					auto grass = new (grasses + x * S_GROUNDS + z) Renderable::XZUniformN::Grass::Builder(device, ground);
				}
			}
		}

		~GroundRenderSystem()
		{
			delete groundPipeline;
			delete grassPipeline;
			vkDestroyPipelineLayout(device.GetDevice(), groundPipelineLayout, nullptr);
			vkDestroyPipelineLayout(device.GetDevice(), grassPipelineLayout, nullptr);

			for (long x = GROUNDS - 1; x >= 0; x--)
			{
				for (long z = GROUNDS - 1; z >= 0; z--)
				{
					grounds[x * GROUNDS + z].~Builder();
					grasses[x * GROUNDS + z].~Builder();
				}
			}
			operator delete[](grounds);
			operator delete[](grasses);
		}

		GroundRenderSystem(const GroundRenderSystem&) = delete;
		GroundRenderSystem& operator=(const GroundRenderSystem&) = delete;

		Renderable::XZUniformN::Builder* MapWorldToGround(const float world_x, const float world_z) const
		{
			uint32_t i_x = static_cast<uint32_t>(world_x / (Renderable::XZUniformN::QUADS * Renderable::XZUniformN::QUAD_SIZE) + GROUNDS / 2);
			uint32_t i_z = static_cast<uint32_t>(world_z / (Renderable::XZUniformN::QUADS * Renderable::XZUniformN::QUAD_SIZE) + GROUNDS / 2);
			return &grounds[i_x * GROUNDS + i_z];
		}

		float MapWorldToHeight(const float world_x, const float world_z) const
		{
			const auto ground = MapWorldToGround(world_x, world_z);
			return ground->MapWorldToHeight(world_x, world_z);
		}

		Math::Vector3 MapWorldToNormal(const float world_x, const float world_z) const
		{
			const auto ground = MapWorldToGround(world_x, world_z);
			return ground->MapWorldToNormal(world_x, world_z);
		}

		void Render(State::FrameInfo& frameInfo)
		{
			RenderGround(frameInfo);
			RenderGrass(frameInfo);
		}

	private:
		void RenderGround(State::FrameInfo& frameInfo)
		{
			groundPipeline->Bind(frameInfo.commandBuffer);

			vkCmdBindDescriptorSets(
				frameInfo.commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				groundPipelineLayout,
				0,
				1,
				&frameInfo.globalDescriptorSet,
				0,
				nullptr
			);

			for (long x = 0; x < GROUNDS; x++)
			{
				for (long z = 0; z < GROUNDS; z++)
				{
					Renderable::XZUniformN::Builder* ground = &grounds[x * GROUNDS + z];

					vkCmdPushConstants(
						frameInfo.commandBuffer,
						groundPipelineLayout,
						VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
						0,
						sizeof(Renderable::XZUniformN::XZPositionalData),
						&(ground->positionalData)
					);
					ground->Bind(frameInfo.commandBuffer);
					ground->Draw(frameInfo.commandBuffer);
				}
			}
		}

		void RenderGrass(State::FrameInfo& frameInfo)
		{
			grassPipeline->Bind(frameInfo.commandBuffer);

			vkCmdBindDescriptorSets(
				frameInfo.commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				grassPipelineLayout,
				0,
				1,
				&frameInfo.globalDescriptorSet,
				0,
				nullptr
			);

			for (long x = 0; x < GROUNDS; x++)
			{
				for (long z = 0; z < GROUNDS; z++)
				{
					Renderable::XZUniformN::Grass::Builder* grass = &grasses[x * GROUNDS + z];

					grass->Bind(frameInfo.commandBuffer);
					grass->Draw(frameInfo.commandBuffer);
				}
			}
		}

		void CreateGroundPipelineLayout(VkDescriptorSetLayout globalSetLayout)
		{
			VkPushConstantRange pushConstantRange{};
			pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			pushConstantRange.offset = 0;
			pushConstantRange.size = sizeof(Renderable::XZUniformN::XZPositionalData);

			const constexpr uint32_t descriptorSetLayoutsLength = 1;
			const VkDescriptorSetLayout descriptorSetLayouts[descriptorSetLayoutsLength]{
				globalSetLayout
			};

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = descriptorSetLayoutsLength;
			pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts;
			pipelineLayoutInfo.pushConstantRangeCount = 1;
			pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
			if (vkCreatePipelineLayout(device.GetDevice(), &pipelineLayoutInfo, nullptr, &groundPipelineLayout) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create pipeline layout!");
			}
		}

		void CreateGroundPipeline(VkRenderPass renderPass)
		{
			assert(groundPipelineLayout != nullptr && "Cannot create pipeline before a pipeline layout is instantiated");

			PipelineConfigInfo pipelineConfig{};
			Pipeline::PixelPipelineTriangleStripNormalConfigInfo(pipelineConfig);
			pipelineConfig.renderPass = renderPass;
			pipelineConfig.pipelineLayout = groundPipelineLayout;
			groundPipeline = Pipeline::Builder(device)
				.AddShaderModule(
					VK_SHADER_STAGE_VERTEX_BIT,
					Shaders::Ground::VERTEXSHADER_VERT,
					sizeof(Shaders::Ground::VERTEXSHADER_VERT) / sizeof(unsigned char)
				).AddShaderModule(
					VK_SHADER_STAGE_FRAGMENT_BIT,
					Shaders::Ground::FRAGSHADER_FRAG,
					sizeof(Shaders::Ground::FRAGSHADER_FRAG) / sizeof(unsigned char)
				).CreateGraphicsPipeline(pipelineConfig);
		}

		void CreateGrassPipelineLayout(VkDescriptorSetLayout globalSetLayout)
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
			if (vkCreatePipelineLayout(device.GetDevice(), &pipelineLayoutInfo, nullptr, &grassPipelineLayout) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create pipeline layout!");
			}
		}

		void CreateGrassPipeline(VkRenderPass renderPass)
		{
			assert(grassPipelineLayout != nullptr && "Cannot create pipeline before a pipeline layout is instantiated");

			PipelineConfigInfo pipelineConfig{};
			PixelPipelinePointListConfigInfo(pipelineConfig);
			pipelineConfig.renderPass = renderPass;
			pipelineConfig.pipelineLayout = grassPipelineLayout;
			grassPipeline = Pipeline::Builder(device)
				.AddShaderModule(
					VK_SHADER_STAGE_GEOMETRY_BIT,
					Shaders::Grass::GEOMSHADER_GEOM,
					sizeof(Shaders::Grass::GEOMSHADER_GEOM) / sizeof(unsigned char)
				).AddShaderModule(
					VK_SHADER_STAGE_VERTEX_BIT,
					Shaders::Grass::VERTEXSHADER_VERT,
					sizeof(Shaders::Grass::VERTEXSHADER_VERT) / sizeof(unsigned char)
				).AddShaderModule(
					VK_SHADER_STAGE_FRAGMENT_BIT,
					Shaders::Grass::FRAGSHADER_FRAG,
					sizeof(Shaders::Grass::FRAGSHADER_FRAG) / sizeof(unsigned char)
				).CreateGraphicsPipeline(pipelineConfig);
		}

		static void PixelPipelinePointListConfigInfo(PipelineConfigInfo& configInfo)
		{
			Pipeline::PixelPipelineConfigInfo(configInfo);

			configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

			configInfo.bindingDescriptions = Renderable::XZUniformN::Grass::Vertex::GetBindingDescriptions();
			configInfo.attributeDescriptions = Renderable::XZUniformN::Grass::Vertex::GetAttributeDescriptions();
		}

		Device& device;

		Pipeline* groundPipeline;
		Pipeline* grassPipeline;
		VkPipelineLayout groundPipelineLayout;
		VkPipelineLayout grassPipelineLayout;

		Renderable::XZUniformN::Builder* grounds;
		Renderable::XZUniformN::Grass::Builder* grasses;
	};
}
