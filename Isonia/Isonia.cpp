#include "Isonia.h"

#include "Controllers/KeyboardMovementController.h"
#include "Pipeline/Buffer.h"
#include "ECS/Camera.h"
#include "Pipeline/Systems/PointLightSystem.h"
#include "Pipeline/Systems/SimpleRenderSystem.h"

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

namespace Isonia
{
	Isonia::Isonia()
	{
		globalPool = Pipeline::Descriptors::DescriptorPool::Builder(device)
			.setMaxSets(Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		LoadGameObjects();
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
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(Pipeline::SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->DescriptorInfo();
			Pipeline::Descriptors::DescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]
				);
		}

		Pipeline::Systems::SimpleRenderSystem simpleRenderSystem{
			device,
			renderer.GetSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout() };
		Pipeline::Systems::PointLightSystem pointLightSystem{
			device,
			renderer.GetSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout() };
		ECS::Camera camera{};

		auto viewerObject = ECS::GameObject::CreateGameObject();
		viewerObject.transform.translation.z = -2.5f;
		Controllers::KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!window.ShouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime =
				std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.MoveInPlaneXZ(window.GetGLFWwindow(), frameTime, viewerObject);
			camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = renderer.GetAspectRatio();
			camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

			if (auto commandBuffer = renderer.BeginFrame())
			{
				int frameIndex = renderer.getFrameIndex();
				State::FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects };

				// update
				State::GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				pointLightSystem.Update(frameInfo, ubo);
				uboBuffers[frameIndex]->WriteToBuffer(&ubo);
				uboBuffers[frameIndex]->Flush();

				// render
				renderer.BeginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.RenderGameObjects(frameInfo);
				pointLightSystem.Render(frameInfo);
				renderer.EndSwapChainRenderPass(commandBuffer);
				renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(device.GetDevice());
	}

	void Isonia::LoadGameObjects()
	{
		std::shared_ptr<ECS::Model> model = ECS::Model::CreateModelFromFile(device, "Resources/Models/FlatVase.obj");
		auto flatVase = ECS::GameObject::CreateGameObject();
		flatVase.model = model;
		flatVase.transform.translation = { -.5f, .5f, 0.f };
		flatVase.transform.scale = { 3.f, 1.5f, 3.f };
		gameObjects.emplace(flatVase.GetId(), std::move(flatVase));

		model = ECS::Model::CreateModelFromFile(device, "Resources/Models/SmoothVase.obj");
		auto smoothVase = ECS::GameObject::CreateGameObject();
		smoothVase.model = model;
		smoothVase.transform.translation = { .5f, .5f, 0.f };
		smoothVase.transform.scale = { 3.f, 1.5f, 3.f };
		gameObjects.emplace(smoothVase.GetId(), std::move(smoothVase));

		model = ECS::Model::CreateModelFromFile(device, "Resources/Models/Quad.obj");
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
}
