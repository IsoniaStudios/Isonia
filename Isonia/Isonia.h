#pragma once

// internal
#include "Debug/Debug.h"

#include "Controllers/Controllers.h"

#include "Math/Math.h"
#include "Noise/Noise.h"

#include "Pipeline/Pipeline.h"
#include "Renderable/Renderable.h"
#include "State/State.h"

// std
#include <chrono>

namespace Isonia
{
	class Isonia
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
		VkDescriptorSet m_global_descriptor_sets[Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT];
		Pipeline::Buffer* m_ubo_buffers[Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT];
		Pipeline::Buffer* m_clock_buffers[Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT];

		Pipeline::Systems::SimpleRenderSystem* m_simple_render_system;
		Pipeline::Systems::GroundRenderSystem* m_ground_render_system;
		Pipeline::Systems::WaterRenderSystem* m_water_render_system;
		Pipeline::Systems::DebuggerRenderSystem* m_debugger_render_system;

		Renderable::Texture* m_grass_day_palette;
		Renderable::Texture* m_water_day_palette;
		Renderable::Texture* m_grass;
		Renderable::Texture* m_debugger;
		Renderable::Texture* m_cloud;

		Renderable::Complete::Model* m_sphere_model;
		Renderable::Complete::Model* m_prism_models[20];

		Controllers::Player m_player{};

		Pipeline::Window m_window{ width, height, name };
		Pipeline::Device m_device{ m_window };
		Pipeline::PixelRenderer m_renderer{ m_window, m_device };
		Pipeline::Descriptors::DescriptorPool* m_global_pool{};
	};
}
