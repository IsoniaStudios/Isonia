// internal
#include "Isonia.h"

namespace Isonia
{
	Isonia::Isonia()
	{
		auto start_time = std::chrono::high_resolution_clock::now();

		initializeDescriptorPool();
		initializeRenderSystems();
		initializeEntities();
		initializePlayer();

		auto end_time = std::chrono::high_resolution_clock::now();
		float initilize_time = std::chrono::duration<float, std::chrono::milliseconds::period>(end_time - start_time).count();
		std::cout << "Initilize Time: " << initilize_time << " ms" << std::endl;
	}

	Isonia::~Isonia()
	{
		auto start_time = std::chrono::high_resolution_clock::now();

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

		delete m_debugger_render_system;
		delete m_ground_render_system;
		delete m_water_render_system;

		delete m_global_set_layout;
		for (Pipeline::Buffer* buffer : m_clock_buffers) {
			delete buffer;
		}
		for (Pipeline::Buffer* buffer : m_ubo_buffers) {
			delete buffer;
		}
		delete m_global_pool;

		auto end_time = std::chrono::high_resolution_clock::now();
		float initilize_time = std::chrono::duration<float, std::chrono::milliseconds::period>(end_time - start_time).count();
		std::cout << std::endl << "Uninitialize Time: " << initilize_time << " ms" << std::endl;
	}

	void Isonia::run()
	{
		Debug::PerformanceTracker performance_tracker;
		auto current_time = std::chrono::high_resolution_clock::now();
		while (!m_window.shouldClose())
		{
			m_window.pollEvents();

			auto new_time = std::chrono::high_resolution_clock::now();
			float frame_time_s = std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).count();
			current_time = new_time;

			performance_tracker.logFrameTime(frame_time_s);

			m_player.act(&m_window, frame_time_s);

			if (auto command_buffer = m_renderer.beginFrame())
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
				m_ground_render_system->render(&frame_info);
				m_debugger_render_system->render(&frame_info);
				m_water_render_system->render(&frame_info, &m_player.m_camera);
				m_renderer.endSwapChainRenderPass(command_buffer);
				m_renderer.blit(command_buffer, m_player.m_camera.m_sub_pixel_offset);
				m_renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(m_device.getDevice());
	}

	void Isonia::initializeDescriptorPool()
	{
		m_global_pool = (new Pipeline::Descriptors::DescriptorPool::Builder(&m_device, 7u))
			->setMaxSets(Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
			->addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::max_frames_in_flight)
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
		}

		Noise::ConstantScalarWarpNoise cloudWarpNoise{ 5.0f };
		Noise::FractalPerlinNoise cloudNoise{ 69, 3, 2.0f, 0.5f, 0.0f };

		m_grass_day_palette = Renderable::createGrassDayPalette(&m_device);
		m_grass = Renderable::createDebugTexture(&m_device);
		m_debugger = Renderable::createDebugTexture(&m_device);
		m_cloud = Renderable::Texture::createTextureFromNoise(&m_device, &cloudWarpNoise, &cloudNoise, 512, 512);
		m_water_day_palette = Renderable::createWaterDayPalette(&m_device);

		m_global_set_layout = (new Pipeline::Descriptors::DescriptorSetLayout::Builder(&m_device, 7u))
			->addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->addBinding(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			->build();

		for (int i = 0; i < Pipeline::SwapChain::max_frames_in_flight; i++)
		{
			VkDescriptorBufferInfo uboBufferInfo = m_ubo_buffers[i]->descriptorInfo();
			VkDescriptorBufferInfo clockBufferInfo = m_clock_buffers[i]->descriptorInfo();
			VkDescriptorImageInfo grassDayPaletteInfo = m_grass_day_palette->getImageInfo();
			VkDescriptorImageInfo grassInfo = m_grass->getImageInfo();
			VkDescriptorImageInfo debuggerInfo = m_debugger->getImageInfo();
			VkDescriptorImageInfo cloudInfo = m_cloud->getImageInfo();
			VkDescriptorImageInfo waterDayPaletteInfo = m_water_day_palette->getImageInfo();
			(new Pipeline::Descriptors::DescriptorWriter(m_global_set_layout, m_global_pool, 7u))
				->writeBuffer(0, &uboBufferInfo)
				->writeBuffer(1, &clockBufferInfo)
				->writeImage(2, &grassDayPaletteInfo)
				->writeImage(3, &grassInfo)
				->writeImage(4, &debuggerInfo)
				->writeImage(5, &cloudInfo)
				->writeImage(6, &waterDayPaletteInfo)
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
