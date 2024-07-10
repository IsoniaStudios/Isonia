// internal
#include "Isonia.h"

// external
#include <chrono>

namespace Isonia
{
	Isonia::Isonia()
	{
		initializeDescriptorPool();
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
		delete m_debugger_render_system;
		delete m_ground_render_system;
		delete m_water_render_system;

		delete m_global_writer;
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
					m_global_descriptor_sets[frame_index]
				};

				// update
				m_ubo.projection = *m_player.m_camera.getProjection();
				m_ubo.view = *m_player.m_camera.getView();
				m_ubo.inverse_view = *m_player.m_camera.getInverseView();

				m_ubo_buffers[frame_index]->writeToBuffer(&m_ubo);
				m_ubo_buffers[frame_index]->flush();

				m_clock.frame_time_s = frame_time_s;
				m_clock.time_s += frame_time_s;

				m_clock_buffers[frame_index]->writeToBuffer(&m_clock);
				m_clock_buffers[frame_index]->flush();

				// render
				m_renderer.beginSwapChainRenderPass(command_buffer);
				m_ground_render_system->render(&frame_info, &m_player.m_camera);
				m_debugger_render_system->render(&frame_info);
				m_water_render_system->render(&frame_info, &m_player.m_camera);
				m_ui_render_system->render(&frame_info, &m_player.m_camera);
				m_renderer.endSwapChainRenderPass(command_buffer);
				m_renderer.blit(command_buffer, m_player.m_camera.m_sub_pixel_offset);
				m_renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(m_device.getDevice());
	}

	void Isonia::initializeDescriptorPool()
	{
		m_global_pool = (new Pipeline::Descriptors::DescriptorPool(&m_device, 9u))
			->addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->build(Pipeline::SwapChain::max_frames_in_flight);

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
		}

		Renderable::GrassTextureAtlasFactory grassTextureAtlasFactory = Renderable::GrassTextureAtlasFactory{ 9u, 9u, 16u, 16u, 1u };
		m_grass = grassTextureAtlasFactory.instantiateTexture(&m_device, VK_FORMAT_R8_UNORM);
		m_grass_day_palette = Renderable::createGrassDayPalette(&m_device);

		Noise::ConstantScalarWarpNoise cloud_warp_noise{ 5.0f };
		Noise::FractalPerlinNoise cloud_noise{ 69, 3, 2.0f, 0.5f, 0.0f };
		Renderable::Noise4DTextureFactory cloudTextureFactory = Renderable::Noise4DTextureFactory{ &cloud_warp_noise, &cloud_noise, 128u, 128u, 1u };
		m_cloud = cloudTextureFactory.instantiateTexture(&m_device, VK_FORMAT_R8_UNORM, VK_FILTER_LINEAR);

		Noise::ConstantScalarWarpNoise wind_w{ 5.0f };
		Noise::PerlinNoise wind_n{ 69 };
		Noise::CurlNoise wind_noise{ &wind_n, &wind_w };
		Renderable::WarpNoiseTextureFactory windTextureFactory = Renderable::WarpNoiseTextureFactory{ &wind_noise, 128u, 128u, 2u };
		m_wind = windTextureFactory.instantiateTexture(&m_device, VK_FORMAT_R8G8_SNORM, VK_FILTER_LINEAR);

		m_debugger = Renderable::createDebugTexture(&m_device);
		m_water_day_palette = Renderable::createWaterDayPalette(&m_device);
		m_text = Renderable::create3x6PixelFontSingleRowTexture(&m_device);

		m_global_set_layout = (new Pipeline::Descriptors::DescriptorSetLayout(&m_device, 9u))
			->addBinding(0u, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(1u, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(2u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(3u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(4u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(5u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(6u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(7u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(8u, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->build();

		for (int i = 0; i < Pipeline::SwapChain::max_frames_in_flight; i++)
		{
			VkDescriptorBufferInfo ubo_buffer_info = m_ubo_buffers[i]->descriptorInfo();
			VkDescriptorBufferInfo clock_buffer_info = m_clock_buffers[i]->descriptorInfo();
			VkDescriptorImageInfo grass_day_palette_info = m_grass_day_palette->getImageInfo();
			VkDescriptorImageInfo grass_info = m_grass->getImageInfo();
			VkDescriptorImageInfo debugger_info = m_debugger->getImageInfo();
			VkDescriptorImageInfo cloud_info = m_cloud->getImageInfo();
			VkDescriptorImageInfo water_day_palette_info = m_water_day_palette->getImageInfo();
			VkDescriptorImageInfo wind_info = m_wind->getImageInfo();
			VkDescriptorImageInfo text_info = m_text->getImageInfo();
			m_global_writer = (new Pipeline::Descriptors::DescriptorWriter(m_global_set_layout, m_global_pool, 9u))
				->writeBuffer(0u, &ubo_buffer_info)
				->writeBuffer(1u, &clock_buffer_info)
				->writeImage(2u, &grass_day_palette_info)
				->writeImage(3u, &grass_info)
				->writeImage(4u, &debugger_info)
				->writeImage(5u, &cloud_info)
				->writeImage(6u, &water_day_palette_info)
				->writeImage(7u, &wind_info)
				->writeImage(8u, &text_info)
				->build(&m_global_descriptor_sets[i]);
		}
	}

	void Isonia::initializeRenderSystems()
	{
		m_ground_render_system = new Pipeline::RenderSystems::GroundRenderSystem{
			&m_device,
			m_renderer.getSwapChainRenderPass(),
			m_global_set_layout->getDescriptorSetLayout()
		};

		m_water_render_system = new Pipeline::RenderSystems::WaterRenderSystem{
			&m_device,
			m_renderer.getSwapChainRenderPass(),
			m_global_set_layout->getDescriptorSetLayout()
		};

		m_debugger_render_system = new Pipeline::RenderSystems::DebuggerRenderSystem{
			&m_device,
			m_renderer.getSwapChainRenderPass(),
			m_global_set_layout->getDescriptorSetLayout()
		};

		m_ui_render_system = new Pipeline::RenderSystems::UIRenderSystem{
			&m_device,
			m_renderer.getSwapChainRenderPass(),
			m_global_set_layout->getDescriptorSetLayout(),
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
