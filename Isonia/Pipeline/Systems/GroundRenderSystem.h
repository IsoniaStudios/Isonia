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

#include "../../Renderable/PosNorm/Builder.h"
#include "../../Renderable/XZUniform/Builder.h"
#include "../../Renderable/Color/Color.h"

//#include "../../Renderable/Texture.h"

// shaders
#include "../../Shaders/Include/Ground/FragShader_frag.h"
#include "../../Shaders/Include/Ground/VertexShader_vert.h"

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

extern Isonia::ECS::Coordinator gCoordinator;

namespace Isonia::Pipeline::Systems
{
	const std::size_t GROUNDS = 4;
	const std::size_t GROUNDS_COUNT = GROUNDS * GROUNDS;

	const int PALETTE_LENGTH = 17;
	const Renderable::Color::Color PALETTE[PALETTE_LENGTH]
	{
		{ 0, 11, 12 },
		{ 0, 16, 18 },
		{ 7, 25, 20 },
		{ 12, 32, 39 },
		{ 21, 45, 54 },
		{ 22, 63, 71 },
		{ 22, 83, 89 },
		{ 28, 91, 64 },
		{ 45, 103, 78 },
		{ 63, 117, 94 },
		{ 81, 139, 115 },
		{ 93, 161, 60 },
		{ 112, 177, 77 },
		{ 140, 197, 66 },
		{ 140, 197, 66 },
		{ 151, 221, 62 },
		{ 215, 224, 131 }
	};

	class GroundRenderSystem
	{
	public:
		GroundRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device)
		{
			//palette = Renderable::Texture::CreateTextureFromPalette(device, PALETTE, PALETTE_LENGTH);

			CreatePipelineLayout(globalSetLayout);
			CreatePipeline(renderPass);

			Noise::Noise noise{};
			auto GROUNDS_LONG = static_cast<long>(GROUNDS);
			auto QUADS_LONG = static_cast<long>(Renderable::XZUniform::QUADS);
			grounds = static_cast<Renderable::XZUniform::Builder*>(operator new[](sizeof(Renderable::XZUniform::Builder) * GROUNDS_COUNT));
			foliages = static_cast<Renderable::PosNorm::Builder*>(operator new[](sizeof(Renderable::PosNorm::Builder) * GROUNDS_COUNT));
			for (long x = 0; x < GROUNDS; x++)
			{
				for (long z = 0; z < GROUNDS; z++)
				{
					float xOffset = (x - GROUNDS_LONG / 2l) * QUADS_LONG * Renderable::XZUniform::QUAD_SIZE;
					float zOffset = (z - GROUNDS_LONG / 2l) * QUADS_LONG * Renderable::XZUniform::QUAD_SIZE;
					new (grounds + x * GROUNDS_LONG + z) Renderable::XZUniform::Builder(noise, device, xOffset, zOffset);
					new (foliages + x * GROUNDS_LONG + z) Renderable::PosNorm::Builder(&grounds[x * GROUNDS + z]);
				}
			}
		}

		~GroundRenderSystem()
		{
			vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, nullptr);
			delete renderSystemLayout;
			delete pipeline;

			for (long x = GROUNDS - 1; x >= 0; x--)
			{
				for (long z = GROUNDS - 1; z >= 0; z--)
				{
					grounds[x * GROUNDS + z].~Builder();
				}
			}
			operator delete[](grounds);
			//delete palette;
		}

		GroundRenderSystem(const GroundRenderSystem&) = delete;
		GroundRenderSystem& operator=(const GroundRenderSystem&) = delete;

		void RenderGround(State::FrameInfo& frameInfo, VkDescriptorBufferInfo bufferInfo, Descriptors::DescriptorPool& descriptorPool)
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

					/*
					// writing descriptor set each frame can slow performance
					// would be more efficient to implement some sort of caching
					VkDescriptorSet groundDescriptorSet;
					auto imageinfo = palette->GetImageInfo();
					Descriptors::DescriptorWriter(*renderSystemLayout, descriptorPool)
						.WriteBuffer(0, &bufferInfo)
						.WriteImage(1, &imageinfo)
						.Build(groundDescriptorSet);

					vkCmdBindDescriptorSets(
						frameInfo.commandBuffer,
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						pipelineLayout,
						1,
						1,
						&groundDescriptorSet,
						0,
						nullptr
					);
					*/

					vkCmdPushConstants(
						frameInfo.commandBuffer,
						pipelineLayout,
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

	private:
		void InitializePaletteDescriptorPool()
		{
			/*
			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.pNext = nullptr;
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = _descriptorPool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &_singleTextureSetLayout;

			vkAllocateDescriptorSets(_device, &allocInfo, &texturedMat->textureSet);

			VkSamplerCreateInfo samplerInfo = vkinit::sampler_create_info(VK_FILTER_NEAREST);

			VkSampler blockySampler;
			vkCreateSampler(_device, &samplerInfo, nullptr, &blockySampler);

			VkDescriptorImageInfo imageBufferInfo;
			imageBufferInfo.sampler = blockySampler;
			imageBufferInfo.imageView = colorMap->GetImageView();
			imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;



			VkWriteDescriptorSet texture1 = vkinit::write_descriptor_image(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, texturedMat->textureSet, &imageBufferInfo, 0);

			vkUpdateDescriptorSets(_device, 1, &texture1, 0, nullptr);
			*/
		}

		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
		{
			VkPushConstantRange pushConstantRange{};
			pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			pushConstantRange.offset = 0;
			pushConstantRange.size = sizeof(Renderable::XZUniform::XZPositionalData);

			renderSystemLayout = Descriptors::DescriptorSetLayout::Builder(device)
				.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
				.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
				.Build();

			std::vector<VkDescriptorSetLayout> descriptorSetLayouts{
				globalSetLayout,
				renderSystemLayout->GetDescriptorSetLayout()
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
			Pipeline::PixelPipelineTriangleStripConfigInfo(pipelineConfig);
			pipelineConfig.renderPass = renderPass;
			pipelineConfig.pipelineLayout = pipelineLayout;
			pipeline = new Pipeline(
				device,
				Shaders::Ground::VERTEXSHADER_VERT,
				sizeof(Shaders::Ground::VERTEXSHADER_VERT) / sizeof(unsigned char),
				Shaders::Ground::FRAGSHADER_FRAG,
				sizeof(Shaders::Ground::FRAGSHADER_FRAG) / sizeof(unsigned char),
				pipelineConfig
			);
		}

		Device& device;

		Pipeline* pipeline;
		VkPipelineLayout pipelineLayout;
		Descriptors::DescriptorSetLayout* renderSystemLayout;

		//Renderable::Texture* palette;
		Renderable::XZUniform::Builder* grounds;
		Renderable::PosNorm::Builder* foliages;
	};
}
