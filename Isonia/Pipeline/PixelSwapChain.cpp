// internal
#include "Pipeline.h"

// external
#include <stdexcept>
#include <iostream>

namespace Isonia::Pipeline
{
	PixelSwapChain::PixelSwapChain(Device* deviceRef, VkExtent2D windowExtent, VkExtent2D renderExtent)
		: m_device(deviceRef), m_window_extent(windowExtent), m_render_extent(renderExtent), m_old_swap_chain(nullptr)
	{
		init();
	}

	PixelSwapChain::PixelSwapChain(Device* deviceRef, VkExtent2D windowExtent, VkExtent2D renderExtent, PixelSwapChain* previous)
		: m_device(deviceRef), m_window_extent(windowExtent), m_render_extent(renderExtent), m_old_swap_chain(previous)
	{
		init();
	}

	PixelSwapChain::~PixelSwapChain()
	{
		for (unsigned int i = 0; i < m_image_count; i++)
		{
			vkDestroyImageView(m_device->getDevice(), m_swap_chain_image_views[i], nullptr);
		}
		delete[] m_swap_chain_image_views;

		if (m_swap_chain != nullptr)
		{
			// the same as vkDestroyImage(m_device->getDevice(), swapChainImages[i], nullptr);
			vkDestroySwapchainKHR(m_device->getDevice(), m_swap_chain, nullptr);
			m_swap_chain = nullptr;
			delete[] m_swap_chain_images;
		}

		for (unsigned int i = 0; i < m_image_count; i++)
		{
			vkDestroyImageView(m_device->getDevice(), m_color_image_views[i], nullptr);
			vkDestroyImage(m_device->getDevice(), m_color_images[i], nullptr);
			vkFreeMemory(m_device->getDevice(), m_color_image_memorys[i], nullptr);
		}
		delete[] m_color_image_views;
		delete[] m_color_images;
		delete[] m_color_image_memorys;

		for (unsigned int i = 0; i < m_image_count; i++)
		{
			vkDestroyImageView(m_device->getDevice(), m_depth_image_views[i], nullptr);
			vkDestroyImage(m_device->getDevice(), m_depth_images[i], nullptr);
			vkFreeMemory(m_device->getDevice(), m_depth_image_memorys[i], nullptr);
		}
		delete[] m_depth_image_views;
		delete[] m_depth_images;
		delete[] m_depth_image_memorys;

		for (unsigned int i = 0; i < m_image_count; i++)
		{
			vkDestroyFramebuffer(m_device->getDevice(), m_swap_chain_framebuffers[i], nullptr);
		}
		delete[] m_swap_chain_framebuffers;

		vkDestroyRenderPass(m_device->getDevice(), m_render_pass, nullptr);

		// cleanup synchronization objects
		for (unsigned int i = 0; i < max_frames_in_flight; i++)
		{
			vkDestroySemaphore(m_device->getDevice(), m_render_finished_semaphores[i], nullptr);
			vkDestroySemaphore(m_device->getDevice(), m_image_available_semaphores[i], nullptr);
			vkDestroyFence(m_device->getDevice(), m_in_flight_fences[i], nullptr);
		}
	}

	void PixelSwapChain::freeOldPixelSwapChain()
	{
		if (m_old_swap_chain != nullptr)
		{
			delete m_old_swap_chain;
		}
	}

	VkImage PixelSwapChain::getSwapChainImage(int index) const
	{
		return m_swap_chain_images[index];
	}
	VkImage PixelSwapChain::getImage(int index) const
	{
		return m_color_images[index];
	}

	VkFramebuffer PixelSwapChain::getFrameBuffer(int index) const
	{
		return m_swap_chain_framebuffers[index];
	}
	VkRenderPass PixelSwapChain::getRenderPass() const
	{
		return m_render_pass;
	}
	VkImageView PixelSwapChain::getImageView(int index) const
	{
		return m_color_image_views[index];
	}
	unsigned int PixelSwapChain::getImageCount() const
	{
		return m_image_count;
	}

	VkFormat PixelSwapChain::getPixelSwapChainImageFormat() const
	{
		return m_swap_chain_image_format;
	}
	VkExtent2D PixelSwapChain::getPixelSwapChainExtent() const
	{
		return m_swap_chain_extent;
	}
	unsigned int PixelSwapChain::getSwapChainWidth() const
	{
		return m_swap_chain_extent.width;
	}
	unsigned int PixelSwapChain::getSwapChainHeight() const
	{
		return m_swap_chain_extent.height;
	}

	VkExtent2D PixelSwapChain::getRenderExtent() const
	{
		return m_render_extent;
	}
	unsigned int PixelSwapChain::getRenderWidth() const
	{
		return m_render_extent.width;
	}
	unsigned int PixelSwapChain::getRenderHeight() const
	{
		return m_render_extent.height;
	}

	float PixelSwapChain::getExtentAspectRatio() const
	{
		return static_cast<float>(getRenderWidth()) / static_cast<float>(getRenderHeight());
	}

	VkFormat PixelSwapChain::findDepthFormat()
	{
		const VkFormat candidates[] { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
		return m_device->findSupportedFormat(
			candidates,
			3u,
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	VkResult PixelSwapChain::acquireNextImage(unsigned int* imageIndex)
	{
		vkWaitForFences(
			m_device->getDevice(),
			1,
			&m_in_flight_fences[m_current_frame],
			VK_TRUE,
			Math::unsigned_long_max
		);

		VkResult result = vkAcquireNextImageKHR(
			m_device->getDevice(),
			m_swap_chain,
			Math::unsigned_long_max,
			m_image_available_semaphores[m_current_frame], // must be a not signaled semaphore
			VK_NULL_HANDLE,
			imageIndex
		);

		return result;
	}

	VkResult PixelSwapChain::submitCommandBuffers(const VkCommandBuffer* buffers, unsigned int* image_index)
	{
		if (m_images_in_flight[*image_index] != VK_NULL_HANDLE)
		{
			vkWaitForFences(m_device->getDevice(), 1, &m_images_in_flight[*image_index], VK_TRUE, UINT64_MAX);
		}
		m_images_in_flight[*image_index] = m_in_flight_fences[m_current_frame];

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_image_available_semaphores[m_current_frame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = buffers;

		VkSemaphore signalSemaphores[] = { m_render_finished_semaphores[m_current_frame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(m_device->getDevice(), 1, &m_in_flight_fences[m_current_frame]);
		VkResult queueSubmitResult = vkQueueSubmit(m_device->getGraphicsQueue(), 1, &submitInfo, m_in_flight_fences[m_current_frame]);
		if (queueSubmitResult != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_swap_chain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = image_index;

		VkResult result = vkQueuePresentKHR(m_device->getPresentQueue(), &presentInfo);

		m_current_frame = (m_current_frame + 1) % max_frames_in_flight;

		return result;
	}

	bool PixelSwapChain::compareSwapFormats(const PixelSwapChain* swap_chain) const
	{
		return swap_chain->m_swap_chain_depth_format == m_swap_chain_depth_format &&
			swap_chain->m_swap_chain_image_format == m_swap_chain_image_format;
	}

	void PixelSwapChain::init()
	{
		createPixelSwapChain();
		createImageViews();
		createRenderPass();
		createColorResources();
		createDepthResources();
		createFramebuffers();
		createSyncObjects();
	}

	void PixelSwapChain::createPixelSwapChain()
	{
		SwapChainSupportDetails swap_chain_support = m_device->getSwapChainSupport();

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swap_chain_support.formats, swap_chain_support.formats_count);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swap_chain_support.present_modes, swap_chain_support.present_modes_count);
		VkExtent2D extent = chooseSwapExtent(&swap_chain_support.capabilities);

		m_image_count = swap_chain_support.capabilities.minImageCount + 1;
		if (swap_chain_support.capabilities.maxImageCount > 0 && m_image_count > swap_chain_support.capabilities.maxImageCount)
		{
			m_image_count = swap_chain_support.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_device->getSurface();

		createInfo.minImageCount = m_image_count;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		QueueFamilyIndices indices = m_device->getPhysicalQueueFamilies();
		unsigned int queueFamilyIndices[] = { indices.graphics_family, indices.present_family };

		if (indices.graphics_family != indices.present_family)
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

		createInfo.preTransform = swap_chain_support.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = m_old_swap_chain == nullptr ? VK_NULL_HANDLE : m_old_swap_chain->m_swap_chain;

		if (vkCreateSwapchainKHR(m_device->getDevice(), &createInfo, nullptr, &m_swap_chain) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain!");
		}

		// we only specified a minimum number of images in the swap chain, so the implementation is
		// allowed to create a swap chain with more. That's why we'll first query the final number of
		// images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
		// retrieve the handles.
		vkGetSwapchainImagesKHR(m_device->getDevice(), m_swap_chain, &m_image_count, nullptr);
		m_swap_chain_images = new VkImage[m_image_count];
		vkGetSwapchainImagesKHR(m_device->getDevice(), m_swap_chain, &m_image_count, m_swap_chain_images);

		m_swap_chain_image_format = surfaceFormat.format;
		m_swap_chain_extent = extent;

		// Transition swap chain images to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR because render pipeline expects it
		for (unsigned int i = 0; i < m_image_count; i++)
		{
			m_device->transitionImageLayout(
				m_swap_chain_images[i],
				m_swap_chain_image_format,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				1,
				1
			);
		}
	}

	void PixelSwapChain::createImageViews()
	{
		m_swap_chain_image_views = new VkImageView[m_image_count];
		for (unsigned int i = 0; i < m_image_count; i++)
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_swap_chain_images[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = m_swap_chain_image_format;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device->getDevice(), &viewInfo, nullptr, &m_swap_chain_image_views[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create texture image view!");
			}
		}
	}

	void PixelSwapChain::createRenderPass()
	{
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = findDepthFormat();
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
		colorAttachment.format = getPixelSwapChainImageFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

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

		VkAttachmentDescription attachments[attachments_length] = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = attachments_length;
		renderPassInfo.pAttachments = attachments;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(m_device->getDevice(), &renderPassInfo, nullptr, &m_render_pass) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create render pass!");
		}
	}

	void PixelSwapChain::createFramebuffers()
	{
		m_swap_chain_framebuffers = new VkFramebuffer[m_image_count];
		for (unsigned int i = 0; i < m_image_count; i++)
		{
			VkImageView attachments[attachments_length] = { m_color_image_views[i], m_depth_image_views[i] };

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_render_pass;
			framebufferInfo.attachmentCount = attachments_length;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = getRenderWidth();
			framebufferInfo.height = getRenderHeight();
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_device->getDevice(), &framebufferInfo, nullptr, &m_swap_chain_framebuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create framebuffer!");
			}
		}
	}

	void PixelSwapChain::createColorResources()
	{
		m_swap_chain_color_format = getPixelSwapChainImageFormat();

		m_color_images = new VkImage[m_image_count];
		m_color_image_memorys = new VkDeviceMemory[m_image_count];
		m_color_image_views = new VkImageView[m_image_count];

		for (unsigned int i = 0; i < m_image_count; i++)
		{
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = getRenderWidth();
			imageInfo.extent.height = getRenderHeight();
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = m_swap_chain_color_format;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.flags = 0;

			m_device->createImageWithInfo(
				&imageInfo,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&m_color_images[i],
				&m_color_image_memorys[i]
			);

			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_color_images[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = m_swap_chain_color_format;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device->getDevice(), &viewInfo, nullptr, &m_color_image_views[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create texture image view!");
			}
		}
	}

	void PixelSwapChain::createDepthResources()
	{
		m_swap_chain_depth_format = findDepthFormat();

		m_depth_images = new VkImage[m_image_count];
		m_depth_image_memorys = new VkDeviceMemory[m_image_count];
		m_depth_image_views = new VkImageView[m_image_count];

		for (unsigned int i = 0; i < m_image_count; i++)
		{
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = getRenderWidth();
			imageInfo.extent.height = getRenderHeight();
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = m_swap_chain_depth_format;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.flags = 0;

			m_device->createImageWithInfo(
				&imageInfo,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&m_depth_images[i],
				&m_depth_image_memorys[i]
			);

			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_depth_images[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = m_swap_chain_depth_format;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device->getDevice(), &viewInfo, nullptr, &m_depth_image_views[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create texture image view!");
			}
		}
	}

	void PixelSwapChain::createSyncObjects()
	{
		m_images_in_flight = new VkFence[m_image_count];
		for (unsigned int i = 0; i < m_image_count; i++)
		{
			m_images_in_flight[i] = VK_NULL_HANDLE;
		}

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (unsigned int i = 0; i < max_frames_in_flight; i++)
		{
			if (vkCreateSemaphore(m_device->getDevice(), &semaphoreInfo, nullptr, &m_image_available_semaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_device->getDevice(), &semaphoreInfo, nullptr, &m_render_finished_semaphores[i]) != VK_SUCCESS ||
				vkCreateFence(m_device->getDevice(), &fenceInfo, nullptr, &m_in_flight_fences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create synchronization objects for a frame!");
			}
		}
	}

	// Helper functions
	VkSurfaceFormatKHR PixelSwapChain::chooseSwapSurfaceFormat(VkSurfaceFormatKHR* available_formats, const unsigned int available_formats_count)
	{
		for (unsigned int i = 0; i < available_formats_count; i++)
		{
			const VkSurfaceFormatKHR available_format = available_formats[i];
			if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return available_format;
			}
		}
		return available_formats[0];
	}

	VkPresentModeKHR PixelSwapChain::chooseSwapPresentMode(VkPresentModeKHR* available_present_modes, const unsigned int available_present_modes_count)
	{
		/*
		for (unsigned int i = 0; i < available_present_modes_count; i++)
		{
			if (available_present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				std::cout << "Present mode: Immediate" << std::endl;
				return available_present_modes[i];
			}
		}
		for (unsigned int i = 0; i < available_present_modes_count; i++)
		{
			if (available_present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				std::cout << "Present mode: Mailbox" << std::endl;
				return available_present_modes[i];
			}
		}
		*/
		std::cout << "Present mode: V-Sync" << std::endl;
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D PixelSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR* capabilities) const
	{
		if (capabilities->currentExtent.width != Math::unsigned_int_max)
		{
			return capabilities->currentExtent;
		}

		return {
			Math::clampui(m_window_extent.width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width),
			Math::clampui(m_window_extent.height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height)
		};
	}
}
