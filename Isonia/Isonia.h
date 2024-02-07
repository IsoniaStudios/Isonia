#pragma once

// external
#define TINYOBJLOADER_IMPLEMENTATION

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// internal
#include "Debug/PerformanceTracker.h"

#include "Controllers/KeyboardMovementController.h"
#include "Window/Window.h"

#include "Pipeline/Renderer.h"
#include "Pipeline/Buffer.h"
#include "Pipeline/Device.h"
#include "Pipeline/Descriptors/Descriptors.h"
#include "Pipeline/Systems/SimpleRenderSystem.h"
#include "Pipeline/Systems/GroundRenderSystem.h"

#include "Physics/PhysicsSystem.h"

#include "Controllers/Player.h"

#include "Renderable/Texture.h"
#include "Renderable/Color/PaletteFactory.h"

#include "ECS/Definitions.h"
#include "ECS/Coordinator.h"
#include "ECS/Coordinator.h"

#include "Components/Camera.h"
#include "Components/Gravity.h"
#include "Components/Mesh.h"
#include "Components/MeshRenderer.h"
#include "Components/RigidBody.h"

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>
#include <random>
#include <memory>
#include <vector>

// test
#define GLFW_EXPOSE_NATIVE_WIN32
#include <Windows.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

// global vars
Isonia::ECS::Coordinator* gCoordinator;

namespace Isonia
{
	class Isonia
	{
	public:
		static constexpr int WIDTH = 1024;
		static constexpr int HEIGHT = 576;
		static constexpr const char* NAME = "Isonia";

		Isonia()
		{
			InitializeDescriptorPool();
			InitializeCoordinator();
			InitializePhysicsSystem();
			InitializeRenderSystems();
			InitializeEntities();
			InitializePlayer();
		}

		~Isonia()
		{
			delete palette;
			delete sphereModel;
			delete groundRenderSystem;
			delete gCoordinator;
			delete globalSetLayout;
			for (Pipeline::Buffer* buffer : uboBuffers) {
				delete buffer;
			}
			delete globalPool;
		}

		Isonia(const Isonia&) = delete;
		Isonia& operator=(const Isonia&) = delete;

		State::GlobalUbo ubo{};
		void Run()
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
					ubo.lightDirection = glm::normalize(ubo.lightDirection - glm::vec3(0, frameTime * 0.01f, 0));
					ubo.projection = player.camera.GetProjection();
					ubo.view = player.camera.GetView();
					ubo.inverseView = player.camera.GetInverseView();
					uboBuffers[frameIndex]->WriteToBuffer(&ubo);
					uboBuffers[frameIndex]->Flush();

					// render
					renderer.BeginSwapChainRenderPass(commandBuffer);
					groundRenderSystem->Render(frameInfo);
					simpleRenderSystem->RenderGameObjects(frameInfo);
					renderer.EndSwapChainRenderPass(commandBuffer);
					renderer.EndFrame();
				}
			}

			vkDeviceWaitIdle(device.GetDevice());
		}

		HWND GetGLFWWindowHandle()
		{
			return glfwGetWin32Window(window.GetGLFWwindow());
		}

	private:
		Renderable::Texture* palette;
		Pipeline::Descriptors::DescriptorSetLayout* globalSetLayout;
		std::vector<VkDescriptorSet> globalDescriptorSets;
		std::vector<Pipeline::Buffer*> uboBuffers;
		void InitializeDescriptorPool()
		{
			globalPool = Pipeline::Descriptors::DescriptorPool::Builder(device)
				.SetMaxSets(Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT)
				.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT)
				.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT)
				.Build();

			uboBuffers.resize(Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT);
			for (int i = 0; i < uboBuffers.size(); i++)
			{
				uboBuffers[i] = new Pipeline::Buffer(
					device,
					sizeof(State::GlobalUbo),
					1,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);
				uboBuffers[i]->Map();
			}

			palette = Renderable::Color::PaletteFactory::CreateGrassDayPalette(device);

			globalSetLayout = Pipeline::Descriptors::DescriptorSetLayout::Builder(device)
				.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
				.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
				.Build();

			globalDescriptorSets.resize(Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT);
			for (int i = 0; i < globalDescriptorSets.size(); i++)
			{
				auto bufferInfo = uboBuffers[i]->DescriptorInfo();
				auto imageInfo = palette->GetImageInfo();
				Pipeline::Descriptors::DescriptorWriter(*globalSetLayout, *globalPool)
					.WriteBuffer(0, &bufferInfo)
					.WriteImage(1, &imageInfo)
					.Build(globalDescriptorSets[i]);
			}
		}

		void InitializeCoordinator()
		{
			gCoordinator = new ECS::Coordinator();

			gCoordinator->RegisterComponent<Components::Transform>();
			gCoordinator->RegisterComponent<Components::Mesh>();
			gCoordinator->RegisterComponent<Components::MeshRenderer>();
			gCoordinator->RegisterComponent<Components::RigidBody>();
			gCoordinator->RegisterComponent<Components::Gravity>();
		}

		Physics::PhysicsSystem* physicsSystem;
		void InitializePhysicsSystem()
		{
			physicsSystem = gCoordinator->RegisterSystem<Physics::PhysicsSystem>();
			{
				ECS::Signature signature;
				signature.set(ECS::GetComponentType<Components::Transform>());
				signature.set(ECS::GetComponentType<Components::RigidBody>());
				signature.set(ECS::GetComponentType<Components::Gravity>());
				gCoordinator->SetSystemSignature<Physics::PhysicsSystem>(signature);
			}
			physicsSystem->Init();
		}

		Pipeline::Systems::SimpleRenderSystem* simpleRenderSystem;
		Pipeline::Systems::GroundRenderSystem* groundRenderSystem;
		void InitializeRenderSystems()
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
		}

		Renderable::Complete::Model* sphereModel;
		void InitializeEntities()
		{
			std::default_random_engine generator;
			std::uniform_real_distribution<float> randPosition(-100.0f, 100.0f);
			std::uniform_real_distribution<float> randRotation(0.0f, 3.0f);
			std::uniform_real_distribution<float> randScale(1.0f, 2.0f);

			sphereModel = Renderable::Complete::Model::CreateModelFromFile(device, "Resources/Models/Sphere.obj");

			for (int i = 0; i < 10; ++i)
			{
				ECS::Entity entity = gCoordinator->CreateEntity();

				gCoordinator->AddComponent(
					entity,
					Components::Transform{
					   glm::vec3{ randPosition(generator), randPosition(generator), randPosition(generator) },
					   glm::vec3{ randRotation(generator), randRotation(generator), randRotation(generator) },
					   glm::vec3{ randScale(generator), randScale(generator), randScale(generator) }
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

			ECS::Entity sphere = gCoordinator->CreateEntity();

			gCoordinator->AddComponent(
				sphere,
				Components::Transform{
				   glm::vec3{ 0, -5, 0 },
				   glm::vec3{ 0 },
				   glm::vec3{ 1 }
				}
			);

			gCoordinator->AddComponent(
				sphere,
				Components::Mesh{ sphereModel }
			);
		}

		void InitializePlayer()
		{
			renderer.RegisterRenderResizeCallback(player.GetOnAspectChangeCallback());
			player.OnAspectChange(&renderer);
		}

		Controllers::Player player{};
		Window::Window window{ WIDTH, HEIGHT, NAME };
		Pipeline::Device device{ window };
		Pipeline::Renderer renderer{ window, device };
		Pipeline::Descriptors::DescriptorPool* globalPool{};
	};
}
