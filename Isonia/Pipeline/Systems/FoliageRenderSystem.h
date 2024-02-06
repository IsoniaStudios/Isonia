#pragma once

// internal
#include "GroundRenderSystem.h"

#include "../Device.h"
#include "../Pipeline.h"
#include "../Descriptors/Descriptors.h"

#include "../../State/FrameInfo.h"

#include "../../ECS/System.h"
#include "../../ECS/Coordinator.h"

#include "../../Components/Mesh.h"
#include "../../Components/Camera.h"
#include "../../Components/Transform.h"

#include "../../Renderable/PosNorm/Builder.h"
#include "../../Renderable/XZUniform/Builder.h"
#include "../../Renderable/Color/Color.h"

// shaders
#include "../../Shaders/Include/Foliage/FragShader_frag.h"
#include "../../Shaders/Include/Foliage/GeomShader_geom.h"
#include "../../Shaders/Include/Foliage/VertexShader_vert.h"

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
	class FoliageRenderSystem
	{
	public:
		FoliageRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device)
		{
			CreatePipelineLayout(globalSetLayout);
			CreatePipeline(renderPass);

			Noise::Noise noise{ 69, 0.05f, 3, 2.0f, 0.5f, 0.0f };
			auto GROUNDS_LONG = static_cast<long>(GROUNDS);
			auto QUADS_LONG = static_cast<long>(Renderable::XZUniform::QUADS);
			grounds = static_cast<Renderable::XZUniform::Builder*>(operator new[](sizeof(Renderable::XZUniform::Builder)* GROUNDS_COUNT));
			for (long x = 0; x < GROUNDS; x++)
			{
				for (long z = 0; z < GROUNDS; z++)
				{
					float xOffset = (x - GROUNDS_LONG / 2l) * QUADS_LONG * Renderable::XZUniform::QUAD_SIZE;
					float zOffset = (z - GROUNDS_LONG / 2l) * QUADS_LONG * Renderable::XZUniform::QUAD_SIZE;
					new (grounds + x * GROUNDS_LONG + z) Renderable::XZUniform::Builder(noise, device, xOffset, zOffset);
				}
			}
		}

		~FoliageRenderSystem()
		{
			delete pipeline;
			vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, nullptr);

			for (long x = GROUNDS - 1; x >= 0; x--)
			{
				for (long z = GROUNDS - 1; z >= 0; z--)
				{
					grounds[x * GROUNDS + z].~Builder();
				}
			}
			operator delete[](grounds);
		}

		FoliageRenderSystem(const FoliageRenderSystem&) = delete;
		FoliageRenderSystem& operator=(const FoliageRenderSystem&) = delete;

		void RenderFoliage(State::FrameInfo& frameInfo)
		{
			pipeline->Bind(frameInfo.commandBuffer);

			vkCmdBindDescriptorSets(
				frameInfo.commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipelineLayout,
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
						pipelineLayout,
						VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT,
						0,
						sizeof(Renderable::XZUniform::XZPositionalData),
						&(ground->positionalData)
					);
					ground->Bind(frameInfo.commandBuffer);
					ground->Draw(frameInfo.commandBuffer);
				}
			}
		}

	private:
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
		{
			VkPushConstantRange pushConstantRange{};
			pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT;
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
			if (vkCreatePipelineLayout(device.GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create pipeline layout!");
			}
		}

		void CreatePipeline(VkRenderPass renderPass)
		{
			assert(pipelineLayout != nullptr && "Cannot create pipeline before a pipeline layout is instantiated");

			PipelineConfigInfo pipelineConfig{};
			Pipeline::PixelPipelinePointListConfigInfo(pipelineConfig);
			pipelineConfig.renderPass = renderPass;
			pipelineConfig.pipelineLayout = pipelineLayout;
			pipeline = Pipeline::Builder(device)
				.AddShaderModule(
					VK_SHADER_STAGE_GEOMETRY_BIT,
					Shaders::Foliage::GEOMSHADER_GEOM,
					sizeof(Shaders::Foliage::GEOMSHADER_GEOM) / sizeof(unsigned char)
				).AddShaderModule(
					VK_SHADER_STAGE_VERTEX_BIT,
					Shaders::Foliage::VERTEXSHADER_VERT,
					sizeof(Shaders::Foliage::VERTEXSHADER_VERT) / sizeof(unsigned char)
				).AddShaderModule(
					VK_SHADER_STAGE_FRAGMENT_BIT,
					Shaders::Foliage::FRAGSHADER_FRAG,
					sizeof(Shaders::Foliage::FRAGSHADER_FRAG) / sizeof(unsigned char)
				).CreateGraphicsPipeline(pipelineConfig);
		}

		Device& device;

		Pipeline* pipeline;
		VkPipelineLayout pipelineLayout;

		Renderable::XZUniform::Builder* grounds;
	};
}
