// internal
#include "Isonia.h"

namespace Isonia
{
	Isonia::Isonia()
	{
		auto start_time = std::chrono::high_resolution_clock::now();

		initializeDescriptorPool();
		initializeCoordinator();
		initializePhysicsSystem();
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

		delete cloud;
		delete debugger;
		delete grass;
		delete grassDayPalette;
		delete waterDayPalette;

		delete sphereModel;
		for (Renderable::Complete::Model* prismModel : prismModels) {
			delete prismModel;
		}

		delete debuggerRenderSystem;
		delete groundRenderSystem;
		delete waterRenderSystem;

		delete gCoordinator;

		delete globalSetLayout;
		for (Pipeline::Buffer* buffer : clockBuffers) {
			delete buffer;
		}
		for (Pipeline::Buffer* buffer : uboBuffers) {
			delete buffer;
		}
		delete globalPool;

		auto end_time = std::chrono::high_resolution_clock::now();
		float initilize_time = std::chrono::duration<float, std::chrono::milliseconds::period>(end_time - start_time).count();
		std::cout << std::endl << "Uninitialize Time: " << initilize_time << " ms" << std::endl;
	}

	void Isonia::run()
	{
		Debug::PerformanceTracker performanceTracker;
		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!window.ShouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			performanceTracker.LogFrameTime(frameTime);

			physicsSystem->Update(frameTime);

			player.Act(window.GetGLFWwindow(), frameTime);

			if (auto commandBuffer = renderer.BeginFrame())
			{
				int frameIndex = renderer.GetFrameIndex();
				State::FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					globalDescriptorSets[frameIndex]
				};

				// update
				//ubo.lightDirection = Math::Normalize(ubo.lightDirection - Math::Vector3(0, frameTime * 0.01f, 0));
				ubo.projection = player.camera.GetProjection();
				ubo.view = player.camera.GetView();
				ubo.inverseView = player.camera.GetInverseView();

				uboBuffers[frameIndex]->WriteToBuffer(&ubo);
				uboBuffers[frameIndex]->Flush();

				clock.frameTime = frameTime;
				clock.time += frameTime;

				clockBuffers[frameIndex]->WriteToBuffer(&clock);
				clockBuffers[frameIndex]->Flush();

				// render
				renderer.BeginSwapChainRenderPass(commandBuffer);
				groundRenderSystem->Render(frameInfo);
				simpleRenderSystem->RenderGameObjects(frameInfo);
				debuggerRenderSystem->Render(frameInfo);
				waterRenderSystem->Render(frameInfo, player.camera);
				renderer.EndSwapChainRenderPass(commandBuffer);
				renderer.Blit(commandBuffer, player.camera.subPixelOffset);
				renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(device.GetDevice());
	}

	void Isonia::initializeDescriptorPool()
	{
		globalPool = Pipeline::Descriptors::DescriptorPool::Builder(device)
			.SetMaxSets(Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();

		for (int i = 0; i < Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			uboBuffers[i] = new Pipeline::Buffer(
				device,
				sizeof(State::GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			uboBuffers[i]->Map();

			clockBuffers[i] = new Pipeline::Buffer(
				device,
				sizeof(State::Clock),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			clockBuffers[i]->Map();
		}

		Noise::ConstantScalarWarpNoise cloudWarpNoise{ 5.0f };
		Noise::FractalPerlinNoise cloudNoise{ 69, 3, 2.0f, 0.5f, 0.0f };

		grassDayPalette = Renderable::Color::PaletteFactory::CreateGrassDayPalette(device);
		waterDayPalette = Renderable::Color::PaletteFactory::CreateWaterDayPalette(device);
		grass = Renderable::Texture::CreateTextureFromFile(device, "Resources/Textures/Grass.png");
		debugger = Renderable::Color::TextureFactory::CreateDebugTexture(device);
		cloud = Renderable::Texture::CreateTextureFromNoise(device, cloudWarpNoise, cloudNoise, 512, 512);

		globalSetLayout = Pipeline::Descriptors::DescriptorSetLayout::Builder(device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.AddBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.AddBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.AddBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.AddBinding(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.Build();

		for (int i = 0; i < Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto uboBufferInfo = uboBuffers[i]->DescriptorInfo();
			auto clockBufferInfo = clockBuffers[i]->DescriptorInfo();
			auto grassDayPaletteInfo = grassDayPalette->GetImageInfo();
			auto grassInfo = grass->GetImageInfo();
			auto debuggerInfo = debugger->GetImageInfo();
			auto cloudInfo = cloud->GetImageInfo();
			auto waterDayPaletteInfo = waterDayPalette->GetImageInfo();
			Pipeline::Descriptors::DescriptorWriter(*globalSetLayout, *globalPool)
				.WriteBuffer(0, &uboBufferInfo)
				.WriteBuffer(1, &clockBufferInfo)
				.WriteImage(2, &grassDayPaletteInfo)
				.WriteImage(3, &grassInfo)
				.WriteImage(4, &debuggerInfo)
				.WriteImage(5, &cloudInfo)
				.WriteImage(6, &waterDayPaletteInfo)
				.Build(globalDescriptorSets[i]);
		}
	}

	void Isonia::initializeRenderSystems()
	{
		simpleRenderSystem = new Pipeline::Systems::SimpleRenderSystem{
			device,
			renderer.GetSwapChainRenderPass(),
			globalSetLayout->GetDescriptorSetLayout()
		};
		gCoordinator->RegisterSystem<Pipeline::Systems::SimpleRenderSystem>(simpleRenderSystem);
		{
			ECS::Signature signature;
			signature.set(ECS::GetComponentType<Components::Transform>());
			signature.set(ECS::GetComponentType<Components::Mesh>());
			gCoordinator->SetSystemSignature<Pipeline::Systems::SimpleRenderSystem>(signature);
		}

		groundRenderSystem = new Pipeline::Systems::GroundRenderSystem{
			device,
			renderer.GetSwapChainRenderPass(),
			globalSetLayout->GetDescriptorSetLayout()
		};

		waterRenderSystem = new Pipeline::Systems::WaterRenderSystem{
			device,
			renderer.GetSwapChainRenderPass(),
			globalSetLayout->GetDescriptorSetLayout()
		};

		debuggerRenderSystem = new Pipeline::Systems::DebuggerRenderSystem{
			device,
			renderer.GetSwapChainRenderPass(),
			globalSetLayout->GetDescriptorSetLayout()
		};
	}

	void Isonia::initializeEntities()
	{
		std::default_random_engine generator;
		std::uniform_real_distribution<float> randPosition(-100.0f, 100.0f);
		std::uniform_real_distribution<float> randRotation(0.0f, 3.0f);
		std::uniform_real_distribution<float> randScale(1.0f, 2.0f);

		sphereModel = Renderable::Complete::Model::CreatePrimitive(device, Renderable::Complete::Primitive::Icosahedron);

		for (int i = 0; i < 0; ++i)
		{
			ECS::Entity entity = gCoordinator->CreateEntity();

			gCoordinator->AddComponent(
				entity,
				Components::Transform{
					Math::Vector3{ randPosition(generator), randPosition(generator), randPosition(generator) },
					Math::Vector3{ randRotation(generator), randRotation(generator), randRotation(generator) },
					Math::Vector3{ randScale(generator), randScale(generator), randScale(generator) }
				}
			);

			gCoordinator->AddComponent(
				entity,
				Components::Mesh{ sphereModel }
			);

			gCoordinator->AddComponent(
				entity,
				Components::RigidBody{}
			);

			gCoordinator->AddComponent<Components::Gravity>(
				entity,
				Components::Gravity{}
			);
		}

		for (uint32_t i = 0; i < 20; i++)
		{
			prismModels[i] = Renderable::Complete::Model::CreatePrimitivePrism(device, i + 3);
			ECS::Entity sphere = gCoordinator->CreateEntity();

			gCoordinator->AddComponent(
				sphere,
				Components::Transform{
					Math::Vector3{ i, -5, 0 },
					Math::Vector3{ 0 },
					Math::Vector3{ 1 }
				}
			);

			gCoordinator->AddComponent(
				sphere,
				Components::Mesh{ prismModels[i] }
			);
		}
	}

	void Isonia::initializePlayer()
	{
		renderer.RegisterRenderResizeCallback(player.GetOnAspectChangeCallback());
		player.OnAspectChange(&renderer);
	}
};
}
