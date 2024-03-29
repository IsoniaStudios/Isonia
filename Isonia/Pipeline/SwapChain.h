#pragma once

#include "Device.h"

// external
#include <vulkan/vulkan.h>

// std
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace Isonia::Pipeline
{
	class SwapChain
	{
	public:
		static constexpr const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

		SwapChain(Device& deviceRef, VkExtent2D extent)
			: device(deviceRef), windowExtent(extent)
		{
			oldSwapChain = nullptr;
			Init();
		}

		SwapChain(Device& deviceRef, VkExtent2D extent, SwapChain* previous)
			: device(deviceRef), windowExtent(extent), oldSwapChain(previous)
		{
			Init();
		}

		~SwapChain()
		{
			for (auto imageView : swapChainImageViews)
			{
				vkDestroyImageView(device.GetDevice(), imageView, nullptr);
			}
			swapChainImageViews.clear();

			if (swapChain != nullptr)
			{
				vkDestroySwapchainKHR(device.GetDevice(), swapChain, nullptr);
				swapChain = nullptr;
			}

			for (int i = 0; i < depthImages.size(); i++)
			{
				vkDestroyImageView(device.GetDevice(), depthImageViews[i], nullptr);
				vkDestroyImage(device.GetDevice(), depthImages[i], nullptr);
				vkFreeMemory(device.GetDevice(), depthImageMemorys[i], nullptr);
			}

			for (auto framebuffer : swapChainFramebuffers)
			{
				vkDestroyFramebuffer(device.GetDevice(), framebuffer, nullptr);
			}

			vkDestroyRenderPass(device.GetDevice(), renderPass, nullptr);

			// cleanup synchronization objects
			for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			{
				vkDestroySemaphore(device.GetDevice(), renderFinishedSemaphores[i], nullptr);
				vkDestroySemaphore(device.GetDevice(), imageAvailableSemaphores[i], nullptr);
				vkDestroyFence(device.GetDevice(), inFlightFences[i], nullptr);
			}
		}

		void FreeOldSwapChain()
		{
			if (oldSwapChain != nullptr)
			{
				delete oldSwapChain;
			}
		}

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		VkFramebuffer GetFrameBuffer(int index) const { return swapChainFramebuffers[index]; }
		VkRenderPass GetRenderPass() const { return renderPass; }
		VkImageView GetImageView(int index) const { return swapChainImageViews[index]; }
		uint32_t ImageCount() const { return static_cast<uint32_t>(swapChainImages.size()); }
		VkFormat GetSwapChainImageFormat() const { return swapChainImageFormat; }
		VkExtent2D GetSwapChainExtent() const { return swapChainExtent; }
		uint32_t Width() const { return swapChainExtent.width; }
		uint32_t Height() const { return swapChainExtent.height; }

		float ExtentAspectRatio() const
		{
			return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
		}

		VkFormat FindDepthFormat()
		{
			return device.FindSupportedFormat(
				{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
			);
		}

		VkResult AcquireNextImage(uint32_t* imageIndex)
		{
			vkWaitForFences(
				device.GetDevice(),
				1,
				&inFlightFences[currentFrame],
				VK_TRUE,
				std::numeric_limits<uint64_t>::max()
			);

			VkResult result = vkAcquireNextImageKHR(
				device.GetDevice(),
				swapChain,
				std::numeric_limits<uint64_t>::max(),
				imageAvailableSemaphores[currentFrame],  // must be a not signaled semaphore
				VK_NULL_HANDLE,
				imageIndex
			);

			return result;
		}

		VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex)
		{
			if (imagesInFlight[*imageIndex] != VK_NULL_HANDLE)
			{
				vkWaitForFences(device.GetDevice(), 1, &imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
			}
			imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;

			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = buffers;

			VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			vkResetFences(device.GetDevice(), 1, &inFlightFences[currentFrame]);
			VkResult queueSubmitResult = vkQueueSubmit(device.GraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]);
			if (queueSubmitResult != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to submit draw command buffer!");
			}

			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = signalSemaphores;

			VkSwapchainKHR swapChains[] = { swapChain };
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapChains;

			presentInfo.pImageIndices = imageIndex;

			VkResult result = vkQueuePresentKHR(device.PresentQueue(), &presentInfo);

			currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

			return result;
		}

		bool CompareSwapFormats(const SwapChain& swapChain) const
		{
			return swapChain.swapChainDepthFormat == swapChainDepthFormat &&
				   swapChain.swapChainImageFormat == swapChainImageFormat;
		}

	private:
		void Init()
		{
			CreateSwapChain();
			CreateImageViews();
			CreateRenderPass();
			CreateDepthResources();
			CreateFramebuffers();
			CreateSyncObjects();
		}

		void CreateSwapChain()
		{
			SwapChainSupportDetails swapChainSupport = device.GetSwapChainSupport();

			VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
			VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
			VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

			uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
			if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
			{
				imageCount = swapChainSupport.capabilities.maxImageCount;
			}

			VkSwapchainCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.surface = device.Surface();

			createInfo.minImageCount = imageCount;
			createInfo.imageFormat = surfaceFormat.format;
			createInfo.imageColorSpace = surfaceFormat.colorSpace;
			createInfo.imageExtent = extent;
			createInfo.imageArrayLayers = 1;
			createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			QueueFamilyIndices indices = device.FindPhysicalQueueFamilies();
			uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

			if (indices.graphicsFamily != indices.presentFamily)
			{
				createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				createInfo.queueFamilyIndexCount = 2;
				createInfo.pQueueFamilyIndices = queueFamilyIndices;
			}
			else
			{
				createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				createInfo.queueFamilyIndexCount = 0;      // Optional
				createInfo.pQueueFamilyIndices = nullptr;  // Optional
			}

			createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

			createInfo.presentMode = presentMode;
			createInfo.clipped = VK_TRUE;

			createInfo.oldSwapchain = oldSwapChain == nullptr ? VK_NULL_HANDLE : oldSwapChain->swapChain;

			if (vkCreateSwapchainKHR(device.GetDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create swap chain!");
			}

			// we only specified a minimum number of images in the swap chain, so the implementation is
			// allowed to create a swap chain with more. That's why we'll first query the final number of
			// images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
			// retrieve the handles.
			vkGetSwapchainImagesKHR(device.GetDevice(), swapChain, &imageCount, nullptr);
			swapChainImages.resize(imageCount);
			vkGetSwapchainImagesKHR(device.GetDevice(), swapChain, &imageCount, swapChainImages.data());

			swapChainImageFormat = surfaceFormat.format;
			swapChainExtent = extent;
		}

		void CreateImageViews()
		{
			swapChainImageViews.resize(swapChainImages.size());
			for (size_t i = 0; i < swapChainImages.size(); i++)
			{
				VkImageViewCreateInfo viewInfo{};
				viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				viewInfo.image = swapChainImages[i];
				viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				viewInfo.format = swapChainImageFormat;
				viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				viewInfo.subresourceRange.baseMipLevel = 0;
				viewInfo.subresourceRange.levelCount = 1;
				viewInfo.subresourceRange.baseArrayLayer = 0;
				viewInfo.subresourceRange.layerCount = 1;

				if (vkCreateImageView(device.GetDevice(), &viewInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to create texture image view!");
				}
			}
		}

		void CreateRenderPass()
		{
			VkAttachmentDescription depthAttachment{};
			depthAttachment.format = FindDepthFormat();
			depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkAttachmentReference depthAttachmentRef{};
			depthAttachmentRef.attachment = 1;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkAttachmentDescription colorAttachment = {};
			colorAttachment.format = GetSwapChainImageFormat();
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference colorAttachmentRef = {};
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass = {};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;
			subpass.pDepthStencilAttachment = &depthAttachmentRef;

			VkSubpassDependency dependency = {};
			dependency.dstSubpass = 0;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.srcAccessMask = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

			VkAttachmentDescription attachments[attachmentsLength] = {colorAttachment, depthAttachment};
			VkRenderPassCreateInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = attachmentsLength;
			renderPassInfo.pAttachments = attachments;
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;
			renderPassInfo.dependencyCount = 1;
			renderPassInfo.pDependencies = &dependency;

			if (vkCreateRenderPass(device.GetDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create render pass!");
			}
		}

		void CreateFramebuffers()
		{
			swapChainFramebuffers.resize(ImageCount());
			for (uint32_t i = 0; i < ImageCount(); i++)
			{
				VkImageView attachments[attachmentsLength] = { swapChainImageViews[i], depthImageViews[i] };

				VkExtent2D swapChainExtent = GetSwapChainExtent();
				VkFramebufferCreateInfo framebufferInfo = {};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = renderPass;
				framebufferInfo.attachmentCount = attachmentsLength;
				framebufferInfo.pAttachments = attachments;
				framebufferInfo.width = swapChainExtent.width;
				framebufferInfo.height = swapChainExtent.height;
				framebufferInfo.layers = 1;

				if (vkCreateFramebuffer(device.GetDevice(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to create framebuffer!");
				}
			}
		}

		void CreateDepthResources()
		{
			VkFormat depthFormat = FindDepthFormat();
			swapChainDepthFormat = depthFormat;
			VkExtent2D swapChainExtent = GetSwapChainExtent();

			depthImages.resize(ImageCount());
			depthImageMemorys.resize(ImageCount());
			depthImageViews.resize(ImageCount());

			for (int i = 0; i < depthImages.size(); i++)
			{
				VkImageCreateInfo imageInfo{};
				imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				imageInfo.imageType = VK_IMAGE_TYPE_2D;
				imageInfo.extent.width = swapChainExtent.width;
				imageInfo.extent.height = swapChainExtent.height;
				imageInfo.extent.depth = 1;
				imageInfo.mipLevels = 1;
				imageInfo.arrayLayers = 1;
				imageInfo.format = depthFormat;
				imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
				imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
				imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
				imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				imageInfo.flags = 0;

				device.CreateImageWithInfo(
					imageInfo,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					depthImages[i],
					depthImageMemorys[i]
				);

				VkImageViewCreateInfo viewInfo{};
				viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				viewInfo.image = depthImages[i];
				viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				viewInfo.format = depthFormat;
				viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
				viewInfo.subresourceRange.baseMipLevel = 0;
				viewInfo.subresourceRange.levelCount = 1;
				viewInfo.subresourceRange.baseArrayLayer = 0;
				viewInfo.subresourceRange.layerCount = 1;

				if (vkCreateImageView(device.GetDevice(), &viewInfo, nullptr, &depthImageViews[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to create texture image view!");
				}
			}
		}

		void CreateSyncObjects()
		{
			imagesInFlight.resize(ImageCount(), VK_NULL_HANDLE);

			VkSemaphoreCreateInfo semaphoreInfo = {};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			{
				if (vkCreateSemaphore(device.GetDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
					vkCreateSemaphore(device.GetDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
					vkCreateFence(device.GetDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to create synchronization objects for a frame!");
				}
			}
		}

		// Helper functions
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
		{
			for (const auto& availableFormat : availableFormats)
			{
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					return availableFormat;
				}
			}

			return availableFormats[0];
		}

		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
		{
			for (const auto& availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				{
					std::cout << "Present mode: Immediate" << std::endl;
					return availablePresentMode;
				}
			}

			for (const auto& availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					std::cout << "Present mode: Mailbox" << std::endl;
					return availablePresentMode;
				}
			}

			std::cout << "Present mode: V-Sync" << std::endl;
			return VK_PRESENT_MODE_FIFO_KHR;
		}

		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
		{
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
				return capabilities.currentExtent;

			return {
				std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, windowExtent.width)),
				std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, windowExtent.height))
			};
		}

		static constexpr const uint32_t attachmentsLength = 2;

		VkFormat swapChainImageFormat;
		VkFormat swapChainDepthFormat;
		VkExtent2D swapChainExtent;

		std::vector<VkFramebuffer> swapChainFramebuffers;
		VkRenderPass renderPass;

		std::vector<VkImage> depthImages;
		std::vector<VkDeviceMemory> depthImageMemorys;
		std::vector<VkImageView> depthImageViews;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;

		Device& device;
		VkExtent2D windowExtent;

		VkSwapchainKHR swapChain;
		SwapChain* oldSwapChain;

		VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
		VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
		VkFence inFlightFences[MAX_FRAMES_IN_FLIGHT];
		std::vector<VkFence> imagesInFlight;
		uint32_t currentFrame = 0;
	};
}
