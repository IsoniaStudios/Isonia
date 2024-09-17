// internal
#include "Isonia.h"

// external
#include <chrono>

namespace Isonia
{
	Isonia::Isonia()
	{
		initializeDescriptorPools();
		initializeRenderSystems();
		initializeEntities();
		initializePlayer();
	}

	Isonia::~Isonia()
	{
		delete m_text;
		delete m_wind;
		delete m_cloud;
		delete m_debugger;
		delete m_grass;
		delete m_grass_day_palette;
		delete m_water_day_palette;

		/*
		delete m_sphere_model;
		for (Renderable::Model* prism_model : m_prism_models) {
			delete prism_model;
		}
		*/

		delete m_ui_render_system;
		delete m_water_render_system;
		delete m_debugger_render_system;
		delete m_ground_render_system;

		delete m_global_swapchain_descriptor_manager;
		delete m_global_descriptor_manager;

		delete m_weather_descriptor_manager;
		delete m_ground_descriptor_manager;
		delete m_water_descriptor_manager;
		delete m_text_descriptor_manager;
		delete m_debugger_descriptor_manager;

		for (Pipeline::Buffer* buffer : m_clock_buffers) {
			delete buffer;
		}
		for (Pipeline::Buffer* buffer : m_ubo_buffers) {
			delete buffer;
		}
	}

	void Isonia::run()
	{
		Debug::PerformanceTracker performance_tracker;
		std::chrono::time_point current_time_s = std::chrono::high_resolution_clock::now();
		while (!m_window.m_should_close)
		{
			m_window.pollEvents();
			if (m_window.m_should_close)
			{
				break;
			}

			std::chrono::time_point new_time_s = std::chrono::high_resolution_clock::now();
			float frame_time_s = std::chrono::duration<float, std::chrono::seconds::period>(new_time_s - current_time_s).count();
			current_time_s = new_time_s;

			m_player.act(&m_window, frame_time_s);

			char* performance_text = performance_tracker.logFrameTime(frame_time_s);
			m_ui_render_system->update(m_renderer.getExtent(), performance_text);
			delete performance_text;

			if (VkCommandBuffer command_buffer = m_renderer.beginFrame())
			{
				int frame_index = m_renderer.getFrameIndex();
				State::FrameInfo frame_info{
					frame_index,
					frame_time_s,
					command_buffer,
					*m_global_descriptor_manager->getDescriptorSets(frame_index),
					*m_global_swapchain_descriptor_manager->getDescriptorSets(frame_index)
				};

				// update
				m_ubo.projection = *m_player.m_camera.getProjection();
				m_ubo.view = *m_player.m_camera.getView();
				m_ubo.inverse_view = *m_player.m_camera.getInverseView();
				m_ubo.sub_pixel_offset = m_player.m_camera.m_sub_pixel_offset;

				m_ubo_buffers[frame_index]->writeToBuffer(&m_ubo);
				m_ubo_buffers[frame_index]->flush();

				m_clock.frame_time_s = frame_time_s;
				m_clock.time_s += frame_time_s;

				m_clock_buffers[frame_index]->writeToBuffer(&m_clock);
				m_clock_buffers[frame_index]->flush();

				// render
				m_renderer.beginSwapChainRenderPass(command_buffer, 0u);
				m_ground_render_system->render(
					m_ground_descriptor_manager->getDescriptorSets(frame_index),
					m_weather_descriptor_manager->getDescriptorSets(frame_index),
					&frame_info,
					&m_player.m_camera
				);
				m_debugger_render_system->render(m_debugger_descriptor_manager->getDescriptorSets(frame_index), &frame_info);
				m_renderer.endSwapChainRenderPass(command_buffer);

				m_renderer.copyToIntermediates(command_buffer);

				m_renderer.beginSwapChainRenderPass(command_buffer, 1u);
				m_water_render_system->render(
					m_water_descriptor_manager->getDescriptorSets(frame_index),
					&frame_info,
					&m_player.m_camera
				);
				m_ui_render_system->render(m_text_descriptor_manager->getDescriptorSets(frame_index), &frame_info, &m_player.m_camera);
				m_renderer.endSwapChainRenderPass(command_buffer);

				m_renderer.blit(command_buffer, m_player.m_camera.m_sub_pixel_offset);
				m_renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(m_device.getDevice());
	}

	void Isonia::initializeDescriptorPools()
	{
		initializeGlobalDescriptorPool();
		initializeSwapChainDescriptorPool();
		initializeWeatherDescriptorPool();
		initializeGroundDescriptorPool();
		initializeWaterDescriptorPool();
		initializeTextDescriptorPool();
		initializeDebuggerDescriptorPool();
	}

	void Isonia::initializeGlobalDescriptorPool()
	{
		m_global_descriptor_manager = new Pipeline::Descriptors::DescriptorManager(&m_device, 2u);
		m_global_descriptor_manager->getPool()
			->addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Pipeline::SwapChain::max_frames_in_flight)
			->build(Pipeline::SwapChain::max_frames_in_flight);
		m_global_descriptor_manager->getSetLayout()
			->addBinding(0u, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(1u, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->build();

		for (int i = 0; i < Pipeline::SwapChain::max_frames_in_flight; i++)
		{
			m_ubo_buffers[i] = new Pipeline::Buffer(
				&m_device,
				sizeof(State::GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			m_ubo_buffers[i]->map();

			m_clock_buffers[i] = new Pipeline::Buffer(
				&m_device,
				sizeof(State::Clock),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			m_clock_buffers[i]->map();

			const VkDescriptorBufferInfo* ubo_buffer_info = m_ubo_buffers[i]->getDescriptorInfo();
			const VkDescriptorBufferInfo* clock_buffer_info = m_clock_buffers[i]->getDescriptorInfo();

			m_global_descriptor_manager->getWriters(i)
				->writeBuffer(0u, ubo_buffer_info)
				->writeBuffer(1u, clock_buffer_info)
				->build(m_global_descriptor_manager->getDescriptorSets(i));
		}
	}

	void Isonia::initializeSwapChainDescriptorPool()
	{
		m_global_swapchain_descriptor_manager = new Pipeline::Descriptors::DescriptorManager(&m_device, 2u);
		m_global_swapchain_descriptor_manager->getPool()
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->build(Pipeline::SwapChain::max_frames_in_flight);
		m_global_swapchain_descriptor_manager->getSetLayout()
			->addBinding(0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(1u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->build();

		for (int i = 0; i < Pipeline::SwapChain::max_frames_in_flight; i++)
		{
			const VkDescriptorImageInfo* intermediate_color_buffer_image_info = m_renderer.getPixelSwapChain()->getIntermediateImageInfo(i);
			const VkDescriptorImageInfo* intermediate_depth_buffer_image_info = m_renderer.getPixelSwapChain()->getIntermediateDepthImageInfo(i);

			m_global_swapchain_descriptor_manager->getWriters(i)
				->writeImage(0u, intermediate_color_buffer_image_info)
				->writeImage(1u, intermediate_depth_buffer_image_info)
				->build(m_global_swapchain_descriptor_manager->getDescriptorSets(i));
		}
	}

	void Isonia::initializeWeatherDescriptorPool()
	{
		m_weather_descriptor_manager = new Pipeline::Descriptors::DescriptorManager(&m_device, 2u);
		m_weather_descriptor_manager->getPool()
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->build(Pipeline::SwapChain::max_frames_in_flight);
		m_weather_descriptor_manager->getSetLayout()
			->addBinding(0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(1u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->build();

		Noise::ConstantScalarWarpNoise cloud_warp_noise{ 5.0f };
		Noise::FractalPerlinNoise cloud_noise{ 69, 3, 2.0f, 0.5f, 0.0f };
		Renderable::Noise4DTextureFactory cloudTextureFactory = Renderable::Noise4DTextureFactory{ &cloud_warp_noise, &cloud_noise, 128u, 128u, 1u };
		m_cloud = cloudTextureFactory.instantiateTexture(&m_device, VK_FORMAT_R8_UNORM, VK_FILTER_LINEAR);

		Noise::ConstantScalarWarpNoise wind_w{ 5.0f };
		Noise::PerlinNoise wind_n{ 69 };
		Noise::CurlNoise wind_noise{ &wind_n, &wind_w };
		Renderable::WarpNoiseTextureFactory windTextureFactory = Renderable::WarpNoiseTextureFactory{ &wind_noise, 128u, 128u, 2u };
		m_wind = windTextureFactory.instantiateTexture(&m_device, VK_FORMAT_R8G8_SNORM, VK_FILTER_LINEAR);

		const VkDescriptorImageInfo* cloud_info = m_cloud->getImageInfo();
		const VkDescriptorImageInfo* wind_info = m_wind->getImageInfo();
		for (int i = 0; i < Pipeline::SwapChain::max_frames_in_flight; i++)
		{
			m_weather_descriptor_manager->getWriters(i)
				->writeImage(0u, cloud_info)
				->writeImage(1u, wind_info)
				->build(m_weather_descriptor_manager->getDescriptorSets(i));
		}
	}

	void Isonia::initializeGroundDescriptorPool()
	{
		m_ground_descriptor_manager = new Pipeline::Descriptors::DescriptorManager(&m_device, 2u);
		m_ground_descriptor_manager->getPool()
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->build(Pipeline::SwapChain::max_frames_in_flight);
		m_ground_descriptor_manager->getSetLayout()
			->addBinding(0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(1u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->build();

		Renderable::GrassTextureAtlasFactory grassTextureAtlasFactory = Renderable::GrassTextureAtlasFactory{ 9u, 9u, 16u, 16u, 1u };
		m_grass = grassTextureAtlasFactory.instantiateTexture(&m_device, VK_FORMAT_R8_UNORM);
		m_grass_day_palette = Renderable::createGrassDayPalette(&m_device);

		const VkDescriptorImageInfo* grass_day_palette_info = m_grass_day_palette->getImageInfo();
		const VkDescriptorImageInfo* grass_info = m_grass->getImageInfo();
		for (int i = 0; i < Pipeline::SwapChain::max_frames_in_flight; i++)
		{
			m_ground_descriptor_manager->getWriters(i)
				->writeImage(0u, grass_day_palette_info)
				->writeImage(1u, grass_info)
				->build(m_ground_descriptor_manager->getDescriptorSets(i));
		}
	}

	void Isonia::initializeWaterDescriptorPool()
	{
		m_water_descriptor_manager = new Pipeline::Descriptors::DescriptorManager(&m_device, 1u);
		m_water_descriptor_manager->getPool()
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->build(Pipeline::SwapChain::max_frames_in_flight);
		m_water_descriptor_manager->getSetLayout()
			->addBinding(0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->build();

		m_water_day_palette = Renderable::createWaterDayPalette(&m_device);
		const VkDescriptorImageInfo* water_day_palette_info = m_water_day_palette->getImageInfo();
		for (int i = 0; i < Pipeline::SwapChain::max_frames_in_flight; i++)
		{
			m_water_descriptor_manager->getWriters(i)
				->writeImage(0u, water_day_palette_info)
				->build(m_water_descriptor_manager->getDescriptorSets(i));
		}
	}

	void Isonia::initializeTextDescriptorPool()
	{
		m_text_descriptor_manager = new Pipeline::Descriptors::DescriptorManager(&m_device, 1u);
		m_text_descriptor_manager->getPool()
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->build(Pipeline::SwapChain::max_frames_in_flight);
		m_text_descriptor_manager->getSetLayout()
			->addBinding(0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->build();

		m_text = Renderable::Font::pixelFont3x6(&m_device);

		const VkDescriptorImageInfo* text_info = m_text->getTexture()->getImageInfo();
		for (int i = 0; i < Pipeline::SwapChain::max_frames_in_flight; i++)
		{
			m_text_descriptor_manager->getWriters(i)
				->writeImage(0u, text_info)
				->build(m_text_descriptor_manager->getDescriptorSets(i));
		}
	}

	void Isonia::initializeDebuggerDescriptorPool()
	{
		m_debugger_descriptor_manager = new Pipeline::Descriptors::DescriptorManager(&m_device, 1u);
		m_debugger_descriptor_manager->getPool()
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->build(Pipeline::SwapChain::max_frames_in_flight);
		m_debugger_descriptor_manager->getSetLayout()
			->addBinding(0u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->build();

		m_debugger = Renderable::createDebugTexture(&m_device);

		const VkDescriptorImageInfo* debugger_info = m_debugger->getImageInfo();
		for (int i = 0; i < Pipeline::SwapChain::max_frames_in_flight; i++)
		{
			m_debugger_descriptor_manager->getWriters(i)
				->writeImage(0u, debugger_info)
				->build(m_debugger_descriptor_manager->getDescriptorSets(i));
		}
	}

	void Isonia::initializeRenderSystems()
	{
		m_ground_render_system = new Pipeline::RenderSystems::GroundRenderSystem{
			&m_device,
			m_renderer.getSwapChainRenderPass(0u),
			m_global_descriptor_manager->getSetLayout()->getDescriptorSetLayout(),
			m_ground_descriptor_manager->getSetLayout()->getDescriptorSetLayout(),
			m_weather_descriptor_manager->getSetLayout()->getDescriptorSetLayout(),
			64, 1.0f, 4.0f
		};

		m_debugger_render_system = new Pipeline::RenderSystems::DebuggerRenderSystem{
			&m_device,
			m_renderer.getSwapChainRenderPass(0u),
			m_global_descriptor_manager->getSetLayout()->getDescriptorSetLayout(),
			m_debugger_descriptor_manager->getSetLayout()->getDescriptorSetLayout()
		};

		m_water_render_system = new Pipeline::RenderSystems::WaterRenderSystem{
			&m_device,
			m_renderer.getSwapChainRenderPass(1u),
			m_global_descriptor_manager->getSetLayout()->getDescriptorSetLayout(),
			m_global_swapchain_descriptor_manager->getSetLayout()->getDescriptorSetLayout(),
			m_water_descriptor_manager->getSetLayout()->getDescriptorSetLayout()
		};

		m_ui_render_system = new Pipeline::RenderSystems::UIRenderSystem{
			&m_device,
			m_renderer.getSwapChainRenderPass(1u),
			m_global_descriptor_manager->getSetLayout()->getDescriptorSetLayout(),
			m_text_descriptor_manager->getSetLayout()->getDescriptorSetLayout(),
			m_text,
			128u
		};
	}

	void Isonia::initializeEntities()
	{
	}

	void Isonia::initializePlayer()
	{
		m_renderer.registerRenderResizeCallback(m_player.getOnAspectChangeCallback(), &m_player);
		m_player.onAspectChange(&m_renderer, &m_player);
	}
}
