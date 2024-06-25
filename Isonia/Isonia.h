#pragma once

// internal
#include "Debug/Debug.h"

#include "Controllers/Controllers.h"

#include "Math/Math.h"
#include "Noise/Noise.h"

#include "State/State.h"
#include "Pipeline/Pipeline.h"
#include "Pipeline/Descriptors/Descriptors.h"
#include "Pipeline/RenderSystems/RenderSystems.h"
#include "Renderable/Renderable.h"

namespace Isonia
{
	struct Isonia
	{
	public:
		static constexpr const unsigned int width = 1024;
		static constexpr const unsigned int height = 576;
		static constexpr const char* name = "Isonia";

		Isonia();
		~Isonia();

		Isonia(const Isonia&) = delete;
		Isonia& operator=(const Isonia&) = delete;

		void run();

		State::GlobalUbo m_ubo{};
		State::Clock m_clock{};

	private:
		void initializeDescriptorPool();
		void initializeRenderSystems();
		void initializeEntities();
		void initializePlayer();

		Pipeline::Descriptors::DescriptorSetLayout* m_global_set_layout;
		VkDescriptorSet m_global_descriptor_sets[Pipeline::SwapChain::max_frames_in_flight];
		Pipeline::Buffer* m_ubo_buffers[Pipeline::SwapChain::max_frames_in_flight];
		Pipeline::Buffer* m_clock_buffers[Pipeline::SwapChain::max_frames_in_flight];

		Pipeline::RenderSystems::GroundRenderSystem* m_ground_render_system;
		Pipeline::RenderSystems::WaterRenderSystem* m_water_render_system;
		Pipeline::RenderSystems::DebuggerRenderSystem* m_debugger_render_system;

		Renderable::Texture* m_grass_day_palette;
		Renderable::Texture* m_water_day_palette;
		Renderable::Texture* m_grass;
		Renderable::Texture* m_debugger;
		Renderable::Texture* m_cloud;

		Renderable::Model* m_sphere_model;
		Renderable::Model* m_prism_models[20];

		Controllers::Player m_player{};

		Pipeline::Window m_window{ width, height, name };
		Pipeline::Device m_device{ &m_window };
		Pipeline::PixelRenderer m_renderer{ &m_window, &m_device };
		Pipeline::Descriptors::DescriptorPool* m_global_pool{};
	};
}
