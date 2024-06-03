#pragma once

// internal
#include "Device.h"
#include "PixelSwapChain.h"
#include "../Window/Window.h"

#include "../Math/Generics.h"
#include "../Math/Vector.h"
#include "../Math/Retro.h"

// std
#include <cassert>
#include <memory>
#include <vector>
#include <array>
#include <stdexcept>
#include <functional>
#include <limits>

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

			VkViewport viewport{
				.x = 0.0f,
				.y = 0.0f,
				.width = static_cast<float>(extent.width),
				.height = static_cast<float>(extent.height),
				.minDepth = 0.0f,
				.maxDepth = 1.0f
			};
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

		void Blit(VkCommandBuffer commandBuffer, Math::Vector2 offset)
		{
			// The common subresource thingies
			VkImageSubresourceRange subresourceRange{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			};

			VkImageSubresourceLayers subresource{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			};

			// Transfer the swapchain image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, so we can blit to it
			VkImageMemoryBarrier clearBarrier{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				.pNext = nullptr,
				.srcAccessMask = 0,
				.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.image = pixelSwapChain->GetSwapChainImage(currentImageIndex),
				.subresourceRange = subresourceRange
			};

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &clearBarrier);

			// Calculate pixel offsets
			const float scaleFactor = Math::Retro::PIXELS_PER_UNIT * static_cast<float>(Math::Retro::RenderFactor);
			const int32_t offsetX = Math::RoundToInt(offset.x * scaleFactor);
			const int32_t offsetY = Math::RoundToInt(offset.y * scaleFactor);

			// Define source and destination offsets for image blit
			const int32_t renderFactor = static_cast<int32_t>(Math::Retro::RenderFactor);
			const int32_t srcWidth = static_cast<int32_t>(pixelSwapChain->RenderWidth());
			const int32_t srcHeight = static_cast<int32_t>(pixelSwapChain->RenderHeight());
			const int32_t dstWidth = srcWidth * renderFactor;
			const int32_t dstHeight = srcHeight * renderFactor;

			// Create image blit configuration
			VkImageBlit imageBlit
			{
				.srcSubresource = subresource,
				.srcOffsets = {
					{ 2, 2, 0 },
					{ srcWidth - 2, srcHeight - 2, 1 }
				},
				.dstSubresource = subresource,
				.dstOffsets = {
					{ renderFactor + offsetX, renderFactor + offsetY, 0 },
					{ dstWidth - renderFactor * 3 + offsetX, dstHeight - renderFactor * 3 + offsetY, 1 }
				}
			};

			// Blit the image to the swapchain image
			vkCmdBlitImage(commandBuffer, pixelSwapChain->GetImage(currentImageIndex), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pixelSwapChain->GetSwapChainImage(currentImageIndex), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlit, VK_FILTER_NEAREST);

			// "Blit" the remaining renderFactor * 2 border using compute shader bc of hardware limitations of blit


			// Transfer to the presentation layout
			VkImageMemoryBarrier presentBarrier{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				.pNext = nullptr,
				.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
				.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.image = pixelSwapChain->GetSwapChainImage(currentImageIndex),
				.subresourceRange = subresourceRange
			};

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &presentBarrier);
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

		static void CalculateResolution(VkExtent2D windowExtent, float& outWidth, float& outHeight)
		{
			static const constexpr float idealPixelDensity = 640.0f * 360.0f; //512.0f * 288.0f;

			float closestIdealPixelDensity = std::numeric_limits<float>::max();
			for (uint32_t factor = 1; factor <= 8; factor++)
			{
				const float width = static_cast<float>(windowExtent.width) / static_cast<float>(factor);
				const float height = static_cast<float>(windowExtent.height) / static_cast<float>(factor);
				const float pixelDensity = width * height;
				const float difference = std::abs(idealPixelDensity - pixelDensity);

				if (difference < std::abs(idealPixelDensity - closestIdealPixelDensity))
				{
					closestIdealPixelDensity = pixelDensity;
					outWidth = width;
					outHeight = height;

					Math::Retro::RenderFactor = factor;
				}
			}
		}

		static VkExtent2D RecalculateCameraSettings(VkExtent2D windowExtent)
		{
			float renderWidth; float renderHeight;
			CalculateResolution(windowExtent, renderWidth, renderHeight);

			// extended so we can render sub-pixels
			return {
				static_cast<uint32_t>(Math::FloorToInt(renderWidth)) + 2,
				static_cast<uint32_t>(Math::FloorToInt(renderHeight))+ 2
			};
			// odd number so it snaps as little as posible on camera rotation extended so we can render sub-pixels
			return {
				static_cast<uint32_t>(Math::GetCeiledOddNumber(renderWidth)) + 2,
				static_cast<uint32_t>(Math::GetCeiledOddNumber(renderHeight))+ 2
			};
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

			VkExtent2D renderExtent = RecalculateCameraSettings(windowExtent);

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
