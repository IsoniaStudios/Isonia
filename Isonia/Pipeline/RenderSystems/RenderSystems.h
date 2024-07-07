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
		DebuggerRenderSystem(Device* device, const VkRenderPass render_pass, const VkDescriptorSetLayout global_set_layout);
		~DebuggerRenderSystem();

		DebuggerRenderSystem(const DebuggerRenderSystem&) = delete;
		DebuggerRenderSystem& operator=(const DebuggerRenderSystem&) = delete;

		void render(const State::FrameInfo* frame_info);

	private:
		void createPipelineLayout(const VkDescriptorSetLayout global_set_layout);
		void createPipeline(const VkRenderPass render_pass);

		static void pixelPipelinePointListConfigInfo(PipelineConfigInfo* config_info);

		Device* m_device;

		Pipeline* m_pipeline;
		VkPipelineLayout m_pipeline_layout;

		Renderable::BuilderPosition* m_debugger;
	};

	struct GroundRenderSystem
	{
	public:
		GroundRenderSystem(Device* device, const VkRenderPass render_pass, const VkDescriptorSetLayout global_set_layout);
		~GroundRenderSystem();

		GroundRenderSystem(const GroundRenderSystem&) = delete;
		GroundRenderSystem& operator=(const GroundRenderSystem&) = delete;

		unsigned int mapWorldXToIndex(const float world_x) const;
		unsigned int mapWorldZToIndex(const float world_z) const;
		float mapIndexToWorldX(const unsigned int index) const;
		float mapIndexToWorldZ(const unsigned int index) const;
		Renderable::BuilderXZUniformN* mapWorldToGround(const float world_x, const float world_z) const;
		float mapWorldToHeight(const float world_x, const float world_z) const;
		Math::Vector3 mapWorldToNormal(const float world_x, const float world_z) const;

		void render(const State::FrameInfo* frame_info, const Camera* camera);

	private:
		void frustumCull(const Camera* camera);

		void renderGround(const State::FrameInfo* frame_info);
		void renderGrass(const State::FrameInfo* frame_info);

		void createGroundPipelineLayout(const VkDescriptorSetLayout global_set_layout);
		void createGroundPipeline(const VkRenderPass render_pass);
		void createGrassPipelineLayout(const VkDescriptorSetLayout global_set_layout);
		void createGrassPipeline(const VkRenderPass render_pass);

		static void pixelPipelinePointListConfigInfo(PipelineConfigInfo* config_info);

		Device* m_device;

		Pipeline* m_ground_pipeline;
		Pipeline* m_grass_pipeline;
		VkPipelineLayout m_ground_pipeline_layout;
		VkPipelineLayout m_grass_pipeline_layout;

		static constexpr const unsigned int grounds = 32u;
		static constexpr const unsigned int grounds_count = grounds * grounds;

		Renderable::BuilderXZUniformN* m_grounds[grounds][grounds] = {};
		Renderable::BuilderXZUniformNP* m_grasses[grounds][grounds] = {};

		const Noise::ConstantScalarWarpNoise m_ground_warp_noise{ 0.05f };
		const Noise::FractalPerlinNoise m_ground_noise{ 69, 3, 2.0f, 0.5f, 0.0f };
	};

	struct WaterRenderSystem
	{
	public:
		WaterRenderSystem(Device* device, const VkRenderPass render_pass, const VkDescriptorSetLayout global_set_layout);
		~WaterRenderSystem();

		WaterRenderSystem(const WaterRenderSystem&) = delete;
		WaterRenderSystem& operator=(const WaterRenderSystem&) = delete;

		void render(const State::FrameInfo* frame_info, const Camera* camera);

	private:
		void createpipelineLayout(const VkDescriptorSetLayout global_set_layout);
		void createpipeline(const VkRenderPass render_pass);

		Device* m_device;

		Pipeline* m_pipeline;
		VkPipelineLayout m_pipeline_layout;

		Renderable::BuilderXZUniform* m_water;
	};

	struct UIRenderSystem
	{
	public:
		UIRenderSystem(Device* device, const VkRenderPass render_pass, const VkDescriptorSetLayout global_set_layout, const unsigned int max_text_length);
		~UIRenderSystem();

		UIRenderSystem(const UIRenderSystem&) = delete;
		UIRenderSystem& operator=(const UIRenderSystem&) = delete;

		void update(const VkExtent2D extent, const char* text);

		void render(const State::FrameInfo* frame_info, const Camera* camera);

	private:
		void createPipelineLayout(const VkDescriptorSetLayout global_set_layout);
		void createPipeline(const VkRenderPass render_pass);

		static void pipelineConfigInfo(PipelineConfigInfo* config_info);

		Device* m_device;

		Pipeline* m_pipeline;
		VkPipelineLayout m_pipeline_layout;

		Renderable::BuilderUI* m_ui;
	};
}
