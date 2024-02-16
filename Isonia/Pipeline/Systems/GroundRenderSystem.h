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

#include "../../Renderable/XZUniform/Builder.h"
#include "../../Renderable/XZUniform/Grass/Builder.h"
#include "../../Renderable/Color/Color.h"

#include "../../Noise/FractalPerlinNoise.h"

// shaders
#include "../../Shaders/Include/Ground/FragShader_frag.h"
#include "../../Shaders/Include/Ground/VertexShader_vert.h"

#include "../../Shaders/Include/Grass/FragShader_frag.h"
#include "../../Shaders/Include/Grass/VertexShader_vert.h"
#include "../../Shaders/Include/Grass/GeomShader_geom.h"

// external
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

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
	const std::size_t GROUNDS = 2;
	const std::size_t GROUNDS_COUNT = GROUNDS * GROUNDS;
	const float GRASS_DENSITY = 4.0f;

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

			const long GROUNDS_LONG = static_cast<long>(GROUNDS);
			const long QUADS_LONG = static_cast<long>(Renderable::XZUniform::QUADS);
			grounds = static_cast<Renderable::XZUniform::Builder*>(operator new[](sizeof(Renderable::XZUniform::Builder) * GROUNDS_COUNT));
			grasses = static_cast<Renderable::XZUniform::Grass::Builder*>(operator new[](sizeof(Renderable::XZUniform::Grass::Builder) * GROUNDS_COUNT));
			for (long x = 0; x < GROUNDS; x++)
			{
				for (long z = 0; z < GROUNDS; z++)
				{
					float xOffset = (x - GROUNDS_LONG / 2l) * QUADS_LONG * Renderable::XZUniform::QUAD_SIZE;
					float zOffset = (z - GROUNDS_LONG / 2l) * QUADS_LONG * Renderable::XZUniform::QUAD_SIZE;
					auto ground = new (grounds + x * GROUNDS_LONG + z) Renderable::XZUniform::Builder(device, groundWarpNoise, groundNoise, xOffset, zOffset);
					auto grass = new (grasses + x * GROUNDS_LONG + z) Renderable::XZUniform::Grass::Builder(device, ground, GRASS_DENSITY);
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

		void Render(State::FrameInfo& frameInfo)
		{
			//RenderGround(frameInfo);
			//RenderGrass(frameInfo);
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
					Renderable::XZUniform::Builder* ground = &grounds[x * GROUNDS + z];

					vkCmdPushConstants(
						frameInfo.commandBuffer,
						groundPipelineLayout,
						VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
						0,
						sizeof(Renderable::XZUniform::XZPositionalData),
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
					Renderable::XZUniform::Grass::Builder* grass = &grasses[x * GROUNDS + z];

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
			pushConstantRange.size = sizeof(Renderable::XZUniform::XZPositionalData);

			std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
				globalSetLayout
			};

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
			pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
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
			PixelPipelineTriangleStripConfigInfo(pipelineConfig);
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

		static void PixelPipelineTriangleStripConfigInfo(PipelineConfigInfo& configInfo)
		{
			Pipeline::PixelPipelineConfigInfo(configInfo);

			configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

			configInfo.bindingDescriptions = Renderable::XZUniform::Vertex::GetBindingDescriptions();
			configInfo.attributeDescriptions = Renderable::XZUniform::Vertex::GetAttributeDescriptions();
		}

		void CreateGrassPipelineLayout(VkDescriptorSetLayout globalSetLayout)
		{
			std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
				globalSetLayout
			};

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
			pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
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

			configInfo.bindingDescriptions = Renderable::XZUniform::Grass::Vertex::GetBindingDescriptions();
			configInfo.attributeDescriptions = Renderable::XZUniform::Grass::Vertex::GetAttributeDescriptions();
		}

		Device& device;

		Pipeline* groundPipeline;
		Pipeline* grassPipeline;
		VkPipelineLayout groundPipelineLayout;
		VkPipelineLayout grassPipelineLayout;

		Renderable::XZUniform::Builder* grounds;
		Renderable::XZUniform::Grass::Builder* grasses;
	};
}
