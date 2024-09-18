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
		void initializeDescriptorPools();
		void initializeGlobalDescriptorPool();
		void initializeSwapChainDescriptorPool();
		void initializeWeatherDescriptorPool();
		void initializeGroundDescriptorPool();
		void initializeWaterDescriptorPool();
		void initializeTextDescriptorPool();
		void initializeDebuggerDescriptorPool();

		void initializeRenderSystems();
		void initializeEntities();
		void initializePlayer();

		static void OverwriteSwapChainDescriptorSets(Pipeline::PixelRenderer* renderer, void* descriptor_manager);

		Pipeline::Descriptors::DescriptorManager* m_global_swapchain_descriptor_manager;
		Pipeline::Descriptors::DescriptorManager* m_global_descriptor_manager;

		Pipeline::Descriptors::DescriptorManager* m_weather_descriptor_manager;
		Pipeline::Descriptors::DescriptorManager* m_water_descriptor_manager;
		Pipeline::Descriptors::DescriptorManager* m_ground_descriptor_manager;		
		Pipeline::Descriptors::DescriptorManager* m_text_descriptor_manager;
		Pipeline::Descriptors::DescriptorManager* m_debugger_descriptor_manager;

		Pipeline::Buffer* m_ubo_buffers[Pipeline::SwapChain::max_frames_in_flight];
		Pipeline::Buffer* m_clock_buffers[Pipeline::SwapChain::max_frames_in_flight];

		Pipeline::RenderSystems::GroundRenderSystem* m_ground_render_system;
		Pipeline::RenderSystems::WaterRenderSystem* m_water_render_system;
		Pipeline::RenderSystems::DebuggerRenderSystem* m_debugger_render_system;
		Pipeline::RenderSystems::UIRenderSystem* m_ui_render_system;

		Renderable::Texture* m_grass_day_palette;
		Renderable::Texture* m_water_day_palette;
		Renderable::Texture* m_grass;
		Renderable::Texture* m_debugger;
		Renderable::Texture* m_cloud;
		Renderable::Texture* m_wind;
		Renderable::Font* m_text;

		Renderable::Model* m_sphere_model;
		Renderable::Model* m_prism_models[20];

		//Controllers::Player m_player{};
		Controllers::PlayerIsometric m_player{};

		Pipeline::Window m_window{ width, height, name };
		Pipeline::Device m_device{ &m_window };
		//Pipeline::Renderer m_renderer{ &m_window, &m_device };
		Pipeline::PixelRenderer m_renderer{ &m_window, &m_device };
	};
}
