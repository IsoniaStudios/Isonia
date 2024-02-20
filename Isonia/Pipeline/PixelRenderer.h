#pragma once

// internal
#include "Device.h"
#include "PixelSwapChain.h"
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
	class PixelRenderer
	{
	public:
		PixelRenderer(Window::Window& window, Device& device) : window(window), device(device)
		{
			RecreateSwapChain();
			CreateCommandBuffers();
		}

		~PixelRenderer()
		{
			FreeCommandBuffers();
			pixelSwapChain->FreeOldPixelSwapChain();
			delete pixelSwapChain;
		}

		PixelRenderer(const PixelRenderer&) = delete;
		PixelRenderer& operator=(const PixelRenderer&) = delete;

		using EventHandler = std::function<void(PixelRenderer*)>;
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

		VkRenderPass GetSwapChainRenderPass() const { return pixelSwapChain->GetRenderPass(); }
		float GetAspectRatio() const { return pixelSwapChain->ExtentAspectRatio(); }
		VkExtent2D GetExtent() const { return pixelSwapChain->GetRenderExtent(); }
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

			auto result = pixelSwapChain->AcquireNextImage(&currentImageIndex);
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

			auto result = pixelSwapChain->SubmitCommandBuffers(&commandBuffer, &currentImageIndex);
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
			currentFrameIndex = (currentFrameIndex + 1) % PixelSwapChain::MAX_FRAMES_IN_FLIGHT;
		}

		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
		{
			assert(isFrameStarted && "Can't call begin SwapChainRenderPass if frame is not in progress");
			assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = pixelSwapChain->GetRenderPass();
			renderPassInfo.framebuffer = pixelSwapChain->GetFrameBuffer(currentImageIndex);

			const VkExtent2D extent = pixelSwapChain->GetRenderExtent();
			const VkOffset2D offset = { 0, 0 };

			renderPassInfo.renderArea.offset = offset;
			renderPassInfo.renderArea.extent = extent;

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
			viewport.width = static_cast<float>(extent.width);
			viewport.height = static_cast<float>(extent.height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor{ offset, extent };
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
		}

		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
		{
			assert(isFrameStarted && "Can't call endPixelSwapChainRenderPass if frame is not in progress");
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
			allocInfo.commandBufferCount = PixelSwapChain::MAX_FRAMES_IN_FLIGHT;

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
				PixelSwapChain::MAX_FRAMES_IN_FLIGHT,
				commandBuffers
			);
		}

		void RecreateSwapChain()
		{
			auto windowExtent = window.GetExtent();
			while (windowExtent.width == 0 || windowExtent.height == 0)
			{
				windowExtent = window.GetExtent();
				glfwWaitEvents();
			}
			vkDeviceWaitIdle(device.GetDevice());

			VkExtent2D renderExtent{ 512, 288 };

			if (pixelSwapChain == nullptr)
			{
				pixelSwapChain = new PixelSwapChain(device, windowExtent, renderExtent);
			}
			else
			{
				PixelSwapChain* oldPixelSwapChain = pixelSwapChain;
				oldPixelSwapChain->FreeOldPixelSwapChain();
				pixelSwapChain = new PixelSwapChain(device, windowExtent, renderExtent, oldPixelSwapChain);

				if (!oldPixelSwapChain->CompareSwapFormats(*pixelSwapChain))
				{
					throw std::runtime_error("Swap chain image(or depth) format has changed!");
				}
			}
		}

		Window::Window& window;
		Device& device;
		PixelSwapChain* pixelSwapChain = nullptr;
		VkCommandBuffer commandBuffers[PixelSwapChain::MAX_FRAMES_IN_FLIGHT];
		std::vector<EventHandler> handlers;

		uint32_t currentImageIndex;
		int currentFrameIndex = 0;
		bool isFrameStarted = false;
	};
}
