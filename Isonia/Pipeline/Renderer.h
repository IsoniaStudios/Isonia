#pragma once

// internal
#include "Device.h"
#include "SwapChain.h"
#include "../Window/Window.h"

// std
#include <cassert>
#include <memory>
#include <vector>
#include <array>
#include <stdexcept>
#include <functional>

namespace Isonia::Pipeline
{
	class Renderer
	{
	public:
		Renderer(Window::Window& window, Device& device) : window(window), device(device)
		{
			RecreateSwapChain();
			CreateCommandBuffers();
		}

		~Renderer()
		{
			FreeCommandBuffers();
			swapChain->FreeOldSwapChain();
			delete swapChain;
		}

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		using EventHandler = std::function<void(Renderer*)>;
		void RegisterRenderResizeCallback(EventHandler handler)
		{
			handlers.push_back(handler);
		}

		void PropigateRenderResizeEvent()
		{
			for (const auto& handler : handlers)
			{
				handler(this);
			}
		}

		VkRenderPass GetSwapChainRenderPass() const { return swapChain->GetRenderPass(); }
		float GetAspectRatio() const { return swapChain->ExtentAspectRatio(); }
		VkExtent2D GetExtent() const { return swapChain->GetSwapChainExtent(); }
		bool IsFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer GetCurrentCommandBuffer() const
		{
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int GetFrameIndex() const
		{
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer BeginFrame()
		{
			assert(!isFrameStarted && "Can't call beginFrame while already in progress");

			auto result = swapChain->AcquireNextImage(&currentImageIndex);
			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				RecreateSwapChain();
				return nullptr;
			}

			if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			{
				throw std::runtime_error("Failed to acquire swap chain image!");
			}

			isFrameStarted = true;

			auto commandBuffer = GetCurrentCommandBuffer();
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to begin recording command buffer!");
			}
			return commandBuffer;
		}

		void EndFrame()
		{
			assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
			auto commandBuffer = GetCurrentCommandBuffer();
			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to record command buffer!");
			}

			auto result = swapChain->SubmitCommandBuffers(&commandBuffer, &currentImageIndex);
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.WasWindowResized())
			{
				window.ResetWindowResizedFlag();
				RecreateSwapChain();
				PropigateRenderResizeEvent();
			}
			else if (result != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to present swap chain image!");
			}

			isFrameStarted = false;
			currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
		}

		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
		{
			assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
			assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = swapChain->GetRenderPass();
			renderPassInfo.framebuffer = swapChain->GetFrameBuffer(currentImageIndex);

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapChain->GetSwapChainExtent();

			const uint32_t clearValuesCount = 2;
			VkClearValue clearValues[clearValuesCount]
			{
				{.color = { 0.01f, 0.01f, 0.01f, 1.0f }},
				{.depthStencil = { 1.0f, 0 }}
			};
			renderPassInfo.clearValueCount = clearValuesCount;
			renderPassInfo.pClearValues = clearValues;

			vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(swapChain->GetSwapChainExtent().width);
			viewport.height = static_cast<float>(swapChain->GetSwapChainExtent().height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor{ {0, 0}, swapChain->GetSwapChainExtent() };
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
		}

		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
		{
			assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
			assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");
			vkCmdEndRenderPass(commandBuffer);
		}

	protected:
		void CreateCommandBuffers()
		{
			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = device.GetCommandPool();
			allocInfo.commandBufferCount = SwapChain::MAX_FRAMES_IN_FLIGHT;

			if (vkAllocateCommandBuffers(device.GetDevice(), &allocInfo, commandBuffers) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to allocate command buffers!");
			}
		}

		void FreeCommandBuffers()
		{
			vkFreeCommandBuffers(
				device.GetDevice(),
				device.GetCommandPool(),
				SwapChain::MAX_FRAMES_IN_FLIGHT,
				commandBuffers
			);
		}

		void RecreateSwapChain()
		{
			auto extent = window.GetExtent();
			while (extent.width == 0 || extent.height == 0)
			{
				extent = window.GetExtent();
				glfwWaitEvents();
			}
			vkDeviceWaitIdle(device.GetDevice());

			if (swapChain == nullptr)
			{
				swapChain = new SwapChain(device, extent);
			}
			else
			{
				SwapChain* oldSwapChain = swapChain;
				oldSwapChain->FreeOldSwapChain();
				swapChain = new SwapChain(device, extent, oldSwapChain);

				if (!oldSwapChain->CompareSwapFormats(*swapChain))
				{
					throw std::runtime_error("Swap chain image(or depth) format has changed!");
				}
			}
		}

		Window::Window& window;
		Device& device;
		SwapChain* swapChain = nullptr;
		VkCommandBuffer commandBuffers[SwapChain::MAX_FRAMES_IN_FLIGHT];
		std::vector<EventHandler> handlers;

		uint32_t currentImageIndex;
		int currentFrameIndex = 0;
		bool isFrameStarted = false;
	};
}
