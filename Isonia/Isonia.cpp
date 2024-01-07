#include "Isonia.h"

#include "Controllers/KeyboardMovementController.h"
#include "Pipeline/Buffer.h"
#include "Components/Camera.h"
#include "Pipeline/Systems/SimpleRenderSystem.h"
#include "Physics/PhysicsSystem.h"
#include "ECS/Definitions.h"
#include "ECS/Coordinator.h"
#include "Debug/PerformanceTracker.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>
#include <random>

Isonia::ECS::Coordinator gCoordinator;

namespace Isonia
{
	Isonia::Isonia()
	{
		globalPool = Pipeline::Descriptors::DescriptorPool::Builder(device)
			.SetMaxSets(Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();

		gCoordinator.Init();

		gCoordinator.RegisterComponent<Components::Camera>();
		gCoordinator.RegisterComponent<Components::Gravity>();
		gCoordinator.RegisterComponent<Components::Mesh>();
		gCoordinator.RegisterComponent<Components::MeshRenderer>();
		gCoordinator.RegisterComponent<Components::RigidBody>();
		gCoordinator.RegisterComponent<Components::Transform>();
	}

	Isonia::~Isonia()
	{
	}

	void Isonia::Run()
	{
		std::vector<std::unique_ptr<Pipeline::Buffer>> uboBuffers(Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<Pipeline::Buffer>(
				device,
				sizeof(State::GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			uboBuffers[i]->Map();
		}

		auto globalSetLayout = Pipeline::Descriptors::DescriptorSetLayout::Builder(device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.Build();

		std::vector<VkDescriptorSet> globalDescriptorSets(Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->DescriptorInfo();
			Pipeline::Descriptors::DescriptorWriter(*globalSetLayout, *globalPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(globalDescriptorSets[i]
				);
		}




		auto physicsSystem = gCoordinator.RegisterSystem<Physics::PhysicsSystem>();
		{
			ECS::Signature signature;
			signature.set(gCoordinator.GetComponentType<Components::Gravity>());
			signature.set(gCoordinator.GetComponentType<Components::RigidBody>());
			signature.set(gCoordinator.GetComponentType<Components::Transform>());
			gCoordinator.SetSystemSignature<Physics::PhysicsSystem>(signature);
		}
		physicsSystem->Init();

		Pipeline::Systems::SimpleRenderSystem simpleRenderSystem{
			device,
			renderer.GetSwapChainRenderPass(),
			globalSetLayout->GetDescriptorSetLayout()
		};
		auto test = gCoordinator.RegisterSystem<Pipeline::Systems::SimpleRenderSystem>(&simpleRenderSystem);
		{
			ECS::Signature signature;
			signature.set(gCoordinator.GetComponentType<Components::Mesh>());
			signature.set(gCoordinator.GetComponentType<Components::Transform>());
			gCoordinator.SetSystemSignature<Pipeline::Systems::SimpleRenderSystem>(signature);
		}

		std::vector<ECS::Entity> entities(ECS::MAX_ENTITIES - 1);

		std::default_random_engine generator;
		std::uniform_real_distribution<float> randPosition(-100.0f, 100.0f);
		std::uniform_real_distribution<float> randRotation(0.0f, 3.0f);
		std::uniform_real_distribution<float> randScale(.25f, 1.0f);
		std::uniform_real_distribution<float> randColor(0.0f, 1.0f);
		std::uniform_real_distribution<float> randGravity(-10.0f, -1.0f);

		float scale = randScale(generator);

		Pipeline::Model* model = Pipeline::Model::CreateModelFromFile(device, "Resources/Models/Cube.obj");

		for (auto& entity : entities)
		{
			entity = gCoordinator.CreateEntity();

			gCoordinator.AddComponent<Components::Gravity>(
				entity,
				Components::Gravity{
					.acceleration = glm::vec3(0.0f, randGravity(generator), 0.0f)
				}
			);

			gCoordinator.AddComponent(
				entity,
				Components::RigidBody{
					.velocity = glm::vec3(0.0f, 0.0f, 0.0f),
					.acceleration = glm::vec3(0.0f, 0.0f, 0.0f)
				}
			);

			gCoordinator.AddComponent(
				entity,
				Components::Transform{
				   .position = glm::vec3(randPosition(generator), randPosition(generator), randPosition(generator)),
				   .rotation = glm::vec3(randRotation(generator), randRotation(generator), randRotation(generator)),
				   .scale = glm::vec3(scale, scale, scale)
				}
			);

			gCoordinator.AddComponent(
				entity,
				Components::Mesh{
					.model = model
				}
			);
		}



		Debug::PerformanceTracker performanceTracker;

		Components::Camera camera{};

		auto viewerObject = Components::Transform{};
		viewerObject.position.z = -250.f;
		Controllers::KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!window.ShouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			performanceTracker.LogFrameTime(frameTime);

			physicsSystem->Update(frameTime);

			cameraController.MoveInPlaneXZ(window.GetGLFWwindow(), frameTime, &viewerObject);
			camera.SetViewYXZ(viewerObject.position, viewerObject.rotation);

			float aspect = renderer.GetAspectRatio();
			camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 1000.f);

			if (auto commandBuffer = renderer.BeginFrame())
			{
				int frameIndex = renderer.getFrameIndex();
				State::FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					globalDescriptorSets[frameIndex]
				};

				// update
				State::GlobalUbo ubo{};
				ubo.projection = camera.GetProjection();
				ubo.view = camera.GetView();
				ubo.inverseView = camera.GetInverseView();
				uboBuffers[frameIndex]->WriteToBuffer(&ubo);
				uboBuffers[frameIndex]->Flush();

				// render
				renderer.BeginSwapChainRenderPass(commandBuffer);
				test->RenderGameObjects(frameInfo);
				renderer.EndSwapChainRenderPass(commandBuffer);
				renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(device.GetDevice());
	}
}
