#include "Isonia.h"

#include "Controllers/KeyboardMovementController.h"
#include "Pipeline/Buffer.h"
#include "Components/Camera.h"
#include "Pipeline/Systems/SimpleRenderSystem.h"
#include "Physics/PhysicsSystem.h"
#include "ECS/Definitions.h"

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

		auto physicsSystem = gCoordinator.RegisterSystem<Physics::PhysicsSystem>();
		{
			ECS::Signature signature;
			signature.set(gCoordinator.GetComponentType<Components::Gravity>());
			signature.set(gCoordinator.GetComponentType<Components::RigidBody>());
			signature.set(gCoordinator.GetComponentType<Components::Transform>());
			gCoordinator.SetSystemSignature<Physics::PhysicsSystem>(signature);
		}
		physicsSystem->Init();

		std::vector<ECS::Entity> entities(ECS::MAX_ENTITIES - 1);

		std::default_random_engine generator;
		std::uniform_real_distribution<float> randPosition(-100.0f, 100.0f);
		std::uniform_real_distribution<float> randRotation(0.0f, 3.0f);
		std::uniform_real_distribution<float> randScale(3.0f, 5.0f);
		std::uniform_real_distribution<float> randColor(0.0f, 1.0f);
		std::uniform_real_distribution<float> randGravity(-10.0f, -1.0f);

		float scale = randScale(generator);

		std::shared_ptr<Pipeline::Model> model = Pipeline::Model::CreateModelFromFile(device, "Resources/Models/FlatVase.obj");

		for (auto& entity : entities)
		{
			entity = gCoordinator.CreateEntity();

			gCoordinator.AddComponent<Components::Gravity>(
				entity,
				Components::Gravity{
					glm::vec3(0.0f, randGravity(generator), 0.0f)
				}
			);

			gCoordinator.AddComponent(
				entity,
				Components::RigidBody{
					.velocity = glm::vec3(0.0f, 0.0f, 0.0f),
					.acceleration = glm::vec3(0.0f, 0.0f, 0.0f)
				}
			);

			auto const& transform = Components::Transform{
				.position = glm::vec3(randPosition(generator), randPosition(generator), randPosition(generator)),
				.rotation = glm::vec3(randRotation(generator), randRotation(generator), randRotation(generator)),
				.scale = glm::vec3(scale, scale, scale)
			};
			gCoordinator.AddComponent(
				entity,
				transform
			);

			gCoordinator.AddComponent(
				entity,
				Components::Mesh{
					.model = model,
					.transform = std::make_shared<Components::Transform>(transform)
				}
			);
		}
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

		Pipeline::Systems::SimpleRenderSystem simpleRenderSystem{
			device,
			renderer.GetSwapChainRenderPass(),
			globalSetLayout->GetDescriptorSetLayout() };
		Components::Camera camera{};

		auto viewerObject = Components::Transform{};
		viewerObject.position.z = -2.5f;
		Controllers::KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!window.ShouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.MoveInPlaneXZ(window.GetGLFWwindow(), frameTime, viewerObject);
			camera.SetViewYXZ(viewerObject.position, viewerObject.rotation);

			float aspect = renderer.GetAspectRatio();
			camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

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
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				uboBuffers[frameIndex]->WriteToBuffer(&ubo);
				uboBuffers[frameIndex]->Flush();

				// render
				renderer.BeginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.RenderGameObjects(frameInfo);
				renderer.EndSwapChainRenderPass(commandBuffer);
				renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(device.GetDevice());
	}

	/*
	void Isonia::LoadGameObjects()
	{
		std::shared_ptr<Pipeline::Model> model = Pipeline::Model::CreateModelFromFile(device, "Resources/Models/FlatVase.obj");
		auto flatVase = ECS::GameObject::CreateGameObject();
		flatVase.model = model;
		flatVase.transform.translation = { -.5f, .5f, 0.f };
		flatVase.transform.scale = { 3.f, 1.5f, 3.f };
		gameObjects.emplace(flatVase.GetId(), std::move(flatVase));

		model = Pipeline::Model::CreateModelFromFile(device, "Resources/Models/SmoothVase.obj");
		auto smoothVase = ECS::GameObject::CreateGameObject();
		smoothVase.model = model;
		smoothVase.transform.translation = { .5f, .5f, 0.f };
		smoothVase.transform.scale = { 3.f, 1.5f, 3.f };
		gameObjects.emplace(smoothVase.GetId(), std::move(smoothVase));

		model = Pipeline::Model::CreateModelFromFile(device, "Resources/Models/Quad.obj");
		auto floor = ECS::GameObject::CreateGameObject();
		floor.model = model;
		floor.transform.translation = { 0.f, .5f, 0.f };
		floor.transform.scale = { 3.f, 1.f, 3.f };
		gameObjects.emplace(floor.GetId(), std::move(floor));

		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}
		};

		for (int i = 0; i < lightColors.size(); i++)
		{
			auto pointLight = ECS::GameObject::MakePointLight(0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.f, -1.f, 0.f }
			);
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
			gameObjects.emplace(pointLight.GetId(), std::move(pointLight));
		}
	}
	*/
}
