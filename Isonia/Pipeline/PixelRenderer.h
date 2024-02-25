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

		void Blit(VkCommandBuffer commandBuffer)
		{
			// The common subresource thingies (they must match!)
			VkImageSubresourceRange subresource_range;
			subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresource_range.baseMipLevel = 0;
			subresource_range.levelCount = 1;
			subresource_range.baseArrayLayer = 0;
			subresource_range.layerCount = 1;

			VkImageSubresourceLayers subresource;
			subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresource.mipLevel = 0;
			subresource.baseArrayLayer = 0;
			subresource.layerCount = 1;

			// Transfer the swapchain image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, so we can blit to it
			VkImageMemoryBarrier clear_barrier;
			clear_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			clear_barrier.pNext = nullptr;
			clear_barrier.srcAccessMask = 0;
			clear_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			clear_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			clear_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			clear_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			clear_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			clear_barrier.image = pixelSwapChain->GetSwapChainImage(currentImageIndex);
			clear_barrier.subresourceRange = subresource_range;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &clear_barrier);

			// Transfer the pixel image to VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, so we can blit from it to the swapchain
			VkImageMemoryBarrier blit_barrier;
			blit_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			blit_barrier.pNext = nullptr;
			blit_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			blit_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			blit_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			blit_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			blit_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			blit_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			blit_barrier.image = pixelSwapChain->GetImage(currentImageIndex);
			blit_barrier.subresourceRange = subresource_range;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &blit_barrier);

			// Blit the image to the swapchain image
			VkImageBlit image_blit;
			image_blit.srcSubresource = subresource;
			image_blit.srcOffsets[0] = { };
			image_blit.srcOffsets[1] = { static_cast<int32_t>(pixelSwapChain->RenderWidth()), static_cast<int32_t>(pixelSwapChain->RenderHeight()), 1 };
			image_blit.dstSubresource = subresource;
			image_blit.dstOffsets[0] = { };
			image_blit.dstOffsets[1] = { static_cast<int32_t>(pixelSwapChain->SwapChainWidth()), static_cast<int32_t>(pixelSwapChain->SwapChainHeight()), 1 };

			vkCmdBlitImage(commandBuffer, pixelSwapChain->GetImage(currentImageIndex), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pixelSwapChain->GetSwapChainImage(currentImageIndex), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &image_blit, VK_FILTER_NEAREST);

			// Transfer to the presentation layout
			VkImageMemoryBarrier present_barrier;
			present_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			present_barrier.pNext = nullptr;
			present_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			present_barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			present_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			present_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			present_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			present_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			present_barrier.image = pixelSwapChain->GetSwapChainImage(currentImageIndex);
			present_barrier.subresourceRange = subresource_range;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &present_barrier);
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
