#pragma once

// internal
#include "../Pipeline.h"
#include "../../State/State.h"
#include "../../Renderable/Renderable.h"

// external
#include <vulkan/vulkan.h>

namespace Isonia::Pipeline::RenderSystems
{
	struct DebuggerRenderSystem
	{
	public:
		DebuggerRenderSystem(Device* device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout);
		~DebuggerRenderSystem();

		DebuggerRenderSystem(const DebuggerRenderSystem&) = delete;
		DebuggerRenderSystem& operator=(const DebuggerRenderSystem&) = delete;

		void render(State::FrameInfo* frame_info);

	private:
		void createPipelineLayout(VkDescriptorSetLayout global_set_layout);
		void createPipeline(VkRenderPass render_pass);

		static void pixelPipelinePointListConfigInfo(PipelineConfigInfo* config_info);

		Device* m_device;

		Pipeline* m_pipeline;
		VkPipelineLayout m_pipeline_layout;

		Renderable::BuilderPosition* m_debugger;
	};

	struct GroundRenderSystem
	{
	public:
		GroundRenderSystem(Device* device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout);
		~GroundRenderSystem();

		GroundRenderSystem(const GroundRenderSystem&) = delete;
		GroundRenderSystem& operator=(const GroundRenderSystem&) = delete;

		unsigned int mapWorldXToIndex(const float world_x) const;
		unsigned int mapWorldZToIndex(const float world_z) const;
		Renderable::BuilderXZUniformN* mapWorldToGround(const float world_x, const float world_z) const;
		float mapWorldToHeight(const float world_x, const float world_z) const;
		Math::Vector3 mapWorldToNormal(const float world_x, const float world_z) const;

		void render(const State::FrameInfo* frame_info, const Camera* camera);

	private:
		void frustumCull(const Camera* camera);

		void renderGround(const State::FrameInfo* frame_info);
		void renderGrass(const State::FrameInfo* frame_info);

		void createGroundPipelineLayout(VkDescriptorSetLayout global_set_layout);
		void createGroundPipeline(VkRenderPass render_pass);
		void createGrassPipelineLayout(VkDescriptorSetLayout global_set_layout);
		void createGrassPipeline(VkRenderPass render_pass);

		static void pixelPipelinePointListConfigInfo(PipelineConfigInfo* config_info);

		Device* m_device;

		Pipeline* m_ground_pipeline;
		Pipeline* m_grass_pipeline;
		VkPipelineLayout m_ground_pipeline_layout;
		VkPipelineLayout m_grass_pipeline_layout;

		static constexpr const unsigned int grounds = 32;
		static constexpr const unsigned int grounds_count = grounds * grounds;

		Renderable::BuilderXZUniformN* m_grounds[grounds][grounds] = {};
		Renderable::BuilderXZUniformNP* m_grasses[grounds][grounds] = {};

		const Noise::ConstantScalarWarpNoise m_ground_warp_noise{ 0.05f };
		const Noise::FractalPerlinNoise m_ground_noise{ 69, 3, 2.0f, 0.5f, 0.0f };
	};

	struct WaterRenderSystem
	{
	public:
		WaterRenderSystem(Device* device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout);
		~WaterRenderSystem();

		WaterRenderSystem(const WaterRenderSystem&) = delete;
		WaterRenderSystem& operator=(const WaterRenderSystem&) = delete;

		void render(State::FrameInfo* frame_info, Camera* camera);

	private:
		void createpipelineLayout(VkDescriptorSetLayout global_set_layout);
		void createpipeline(VkRenderPass render_pass);

		Device* m_device;

		Pipeline* m_pipeline;
		VkPipelineLayout m_pipeline_layout;

		Renderable::BuilderXZUniform* m_water;
	};
}
