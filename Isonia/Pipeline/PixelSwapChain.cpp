// internal
#include "Pipeline.h"

// external
#include <stdexcept>
#include <iostream>

namespace Isonia::Pipeline
{
	PixelSwapChain::PixelSwapChain(Device* deviceRef, VkExtent2D window_extent, VkExtent2D render_extent)
		: m_device(deviceRef), m_window_extent(window_extent), m_render_extent(render_extent), m_old_swap_chain(nullptr)
	{
		init();
	}

	PixelSwapChain::PixelSwapChain(Device* deviceRef, VkExtent2D window_extent, VkExtent2D render_extent, PixelSwapChain* previous)
		: m_device(deviceRef), m_window_extent(window_extent), m_render_extent(render_extent), m_old_swap_chain(previous)
	{
		init();
	}

	PixelSwapChain::~PixelSwapChain()
	{
		if (m_swap_chain != nullptr)
		{
			// the same as vkDestroyImage(m_device->getDevice(), swapChainImages[i], nullptr);
			vkDestroySwapchainKHR(m_device->getDevice(), m_swap_chain, nullptr);
			m_swap_chain = nullptr;
		}

		for (unsigned int i = 0; i < m_image_count; i++)
		{
			PixelSwapChainResourceSet* resource = &m_resource_set[i];
			vkDestroyImageView(m_device->getDevice(), resource->m_swap_chain_image_view, nullptr);

			vkDestroyImageView(m_device->getDevice(), resource->m_color_image_view, nullptr);
			vkDestroyImage(m_device->getDevice(), resource->m_color_image, nullptr);
			vkFreeMemory(m_device->getDevice(), resource->m_color_image_memory, nullptr);

			vkDestroyImageView(m_device->getDevice(), resource->m_depth_image_view, nullptr);
			vkDestroyImage(m_device->getDevice(), resource->m_depth_image, nullptr);
			vkFreeMemory(m_device->getDevice(), resource->m_depth_image_memory, nullptr);
		
			vkDestroySemaphore(m_device->getDevice(), resource->m_render_finished_semaphore, nullptr);
			vkDestroySemaphore(m_device->getDevice(), resource->m_image_available_semaphore, nullptr);
			vkDestroyFence(m_device->getDevice(), resource->m_in_flight_fence, nullptr);
		
			vkDestroyImageView(m_device->getDevice(), resource->m_color_image_view_intermediate, nullptr);
			vkDestroyImage(m_device->getDevice(), resource->m_color_image_intermediate, nullptr);
			vkFreeMemory(m_device->getDevice(), resource->m_color_image_memory_intermediate, nullptr);
			vkDestroySampler(m_device->getDevice(), resource->m_color_sampler_intermediate, nullptr);
		
			vkDestroyImageView(m_device->getDevice(), resource->m_depth_image_view_intermediate, nullptr);
			vkDestroyImage(m_device->getDevice(), resource->m_depth_image_intermediate, nullptr);
			vkFreeMemory(m_device->getDevice(), resource->m_depth_image_memory_intermediate, nullptr);
			vkDestroySampler(m_device->getDevice(), resource->m_depth_sampler_intermediate, nullptr);
		}

		for (unsigned int i = 0; i < m_render_pass_count; i++)
		{
			vkDestroyRenderPass(m_device->getDevice(), m_render_passes[i].m_render_pass, nullptr);
			for (unsigned int q = 0; q < m_image_count; q++)
			{
				vkDestroyFramebuffer(m_device->getDevice(), m_render_passes[i].m_framebuffers[q], nullptr);
			}
		}
	}

	void PixelSwapChain::freeOldPixelSwapChain()
	{
		if (m_old_swap_chain != nullptr)
		{
			delete m_old_swap_chain;
		}
	}

	unsigned int PixelSwapChain::getImageCount() const
	{
		return m_image_count;
	}
	VkImage PixelSwapChain::getSwapChainImage(int index) const
	{
		return m_resource_set[index].m_swap_chain_image;
	}
	VkImage PixelSwapChain::getImage(int index) const
	{
		return m_resource_set[index].m_color_image;
	}
	VkImage PixelSwapChain::getDepthImage(int index) const
	{
		return m_resource_set[index].m_depth_image;
	}
	VkImage PixelSwapChain::getIntermediateImage(int index) const
	{
		return m_resource_set[index].m_color_image_intermediate;
	}
	const VkDescriptorImageInfo* PixelSwapChain::getIntermediateImageInfo(int index) const
	{
		return &m_resource_set[index].m_color_descriptor_intermediate;
	}
	VkImage PixelSwapChain::getIntermediateDepthImage(int index) const
	{
		return m_resource_set[index].m_depth_image_intermediate;
	}
	const VkDescriptorImageInfo* PixelSwapChain::getIntermediateDepthImageInfo(int index) const
	{
		return &m_resource_set[index].m_depth_descriptor_intermediate;
	}
	
	VkFramebuffer PixelSwapChain::getFrameBuffer(int index) const
	{
		return m_render_passes[m_current_render_pass_index].m_framebuffers[index];
	}
	VkRenderPass PixelSwapChain::getRenderPass(const unsigned int index) const
	{
		return m_render_passes[index].m_render_pass;
	}
	VkImageView PixelSwapChain::getImageView(int index) const
	{
		return m_resource_set[index].m_color_image_view;
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

	VkResult PixelSwapChain::acquireNextImage(unsigned int* image_index)
	{
		vkWaitForFences(
			m_device->getDevice(),
			1,
			&m_current_resource_set->m_in_flight_fence,
			VK_TRUE,
			Math::unsigned_long_max
		);

		VkResult result = vkAcquireNextImageKHR(
			m_device->getDevice(),
			m_swap_chain,
			Math::unsigned_long_max,
			m_current_resource_set->m_image_available_semaphore, // must be a not signaled semaphore
			nullptr,
			image_index
		);

		return result;
	}

	VkResult PixelSwapChain::submitCommandBuffers(const VkCommandBuffer* buffers, unsigned int* image_index)
	{
		if (m_resource_set[*image_index].m_image_in_flight != nullptr)
		{
			vkWaitForFences(m_device->getDevice(), 1, &m_resource_set[*image_index].m_image_in_flight, VK_TRUE, UINT64_MAX);
		}
		m_resource_set[*image_index].m_image_in_flight = m_current_resource_set->m_in_flight_fence;

		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore wait_semaphores[] = { m_current_resource_set->m_image_available_semaphore };
		VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.pWaitDstStageMask = wait_stages;

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = buffers;

		VkSemaphore signal_semaphores[] = { m_current_resource_set->m_render_finished_semaphore };
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = signal_semaphores;

		vkResetFences(m_device->getDevice(), 1, &m_current_resource_set->m_in_flight_fence);
		VkResult queue_submit_result = vkQueueSubmit(m_device->getGraphicsQueue(), 1, &submit_info, m_current_resource_set->m_in_flight_fence);
		if (queue_submit_result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit draw command buffer!");
		}

		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = signal_semaphores;

		VkSwapchainKHR m_swap_chains[] = { m_swap_chain };
		present_info.swapchainCount = 1;
		present_info.pSwapchains = m_swap_chains;

		present_info.pImageIndices = image_index;

		return vkQueuePresentKHR(m_device->getPresentQueue(), &present_info);
	}

	bool PixelSwapChain::compareSwapFormats(const PixelSwapChain* swap_chain) const
	{
		return swap_chain->m_swap_chain_depth_format == m_swap_chain_depth_format && swap_chain->m_swap_chain_image_format == m_swap_chain_image_format;
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
		createColorIntermediates();
		createDepthIntermediates();
	}

	void PixelSwapChain::createPixelSwapChain()
	{
		SwapChainSupportDetails swap_chain_support = m_device->getSwapChainSupport();

		VkSurfaceFormatKHR surface_format = chooseSwapSurfaceFormat(swap_chain_support.formats, swap_chain_support.formats_count);
		VkPresentModeKHR present_mode = chooseSwapPresentMode(swap_chain_support.present_modes, swap_chain_support.present_modes_count);
		VkExtent2D extent = chooseSwapExtent(&swap_chain_support.capabilities);

		m_image_count = swap_chain_support.capabilities.minImageCount + 1;
		if (swap_chain_support.capabilities.maxImageCount > 0 && m_image_count > swap_chain_support.capabilities.maxImageCount)
		{
			m_image_count = swap_chain_support.capabilities.maxImageCount;
		}
		if (m_image_count > max_frames_in_flight)
		{
			m_image_count = max_frames_in_flight;
		}

		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = m_device->getSurface();

		create_info.minImageCount = m_image_count;
		create_info.imageFormat = surface_format.format;
		create_info.imageColorSpace = surface_format.colorSpace;
		create_info.imageExtent = extent;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		QueueFamilyIndices indices = m_device->getPhysicalQueueFamilies();
		unsigned int queue_family_indices[] = { indices.graphics_family, indices.present_family };

		if (indices.graphics_family != indices.present_family)
		{
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queue_family_indices;
		}
		else
		{
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 0;      // Optional
			create_info.pQueueFamilyIndices = nullptr;  // Optional
		}

		create_info.preTransform = swap_chain_support.capabilities.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;

		create_info.oldSwapchain = m_old_swap_chain == nullptr ? nullptr : m_old_swap_chain->m_swap_chain;

		if (vkCreateSwapchainKHR(m_device->getDevice(), &create_info, nullptr, &m_swap_chain) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain!");
		}

		// we only specified a minimum number of images in the swap chain, so the implementation is
		// allowed to create a swap chain with more. That's why we'll first query the final number of
		// images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
		// retrieve the handles.
		vkGetSwapchainImagesKHR(m_device->getDevice(), m_swap_chain, &m_image_count, nullptr);
		VkImage* m_swap_chain_images = (VkImage*)malloc(m_image_count * sizeof(VkImage));
		vkGetSwapchainImagesKHR(m_device->getDevice(), m_swap_chain, &m_image_count, m_swap_chain_images);

		m_swap_chain_image_format = surface_format.format;
		m_swap_chain_extent = extent;

		// Transition swap chain images to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR because render pipeline expects it
		for (unsigned int i = 0; i < m_image_count; i++)
		{
			m_resource_set[i].m_swap_chain_image = m_swap_chain_images[i];
			m_device->transitionImageLayout(
				m_swap_chain_images[i],
				m_swap_chain_image_format,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				1,
				1
			);
		}
		free(m_swap_chain_images);
	}

	void PixelSwapChain::createImageViews()
	{
		for (unsigned int i = 0; i < m_image_count; i++)
		{
			VkImageViewCreateInfo view_info{};
			view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view_info.image = m_resource_set[i].m_swap_chain_image;
			view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_info.format = m_swap_chain_image_format;
			view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			view_info.subresourceRange.baseMipLevel = 0;
			view_info.subresourceRange.levelCount = 1;
			view_info.subresourceRange.baseArrayLayer = 0;
			view_info.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device->getDevice(), &view_info, nullptr, &m_resource_set[i].m_swap_chain_image_view) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create texture image view!");
			}
		}
	}

	void PixelSwapChain::createRenderPass()
	{
		VkAttachmentDescription depth_attachment{};
		depth_attachment.format = findDepthFormat();
		depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depth_attachment.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		VkAttachmentReference depth_attachment_ref{};
		depth_attachment_ref.attachment = 1;
		depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription color_attachment = {};
		color_attachment.format = getPixelSwapChainImageFormat();
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		VkAttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;
		subpass.pDepthStencilAttachment = &depth_attachment_ref;

		VkSubpassDependency dependency = {};
		dependency.dstSubpass = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcAccessMask = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

		VkAttachmentDescription attachments[attachments_length] = { color_attachment, depth_attachment };
		VkRenderPassCreateInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount = attachments_length;
		render_pass_info.pAttachments = attachments;
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass;
		render_pass_info.dependencyCount = 1;
		render_pass_info.pDependencies = &dependency;

		if (vkCreateRenderPass(m_device->getDevice(), &render_pass_info, nullptr, &m_render_passes[0].m_render_pass) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create render pass!");
		}

		VkAttachmentDescription load_attachments[attachments_length] = { color_attachment, depth_attachment };
		load_attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		load_attachments[0].initialLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		load_attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		load_attachments[1].initialLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		render_pass_info.pAttachments = load_attachments;
		if (vkCreateRenderPass(m_device->getDevice(), &render_pass_info, nullptr, &m_render_passes[1].m_render_pass) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create render pass!");
		}
	}

	void PixelSwapChain::createFramebuffers()
	{
		for (unsigned int r = 0; r < m_render_pass_count; r++)
		{
			for (unsigned int i = 0; i < m_image_count; i++)
			{
				VkImageView attachments[attachments_length] = { m_resource_set[i].m_color_image_view, m_resource_set[i].m_depth_image_view };

				VkFramebufferCreateInfo framebuffer_info = {};
				framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebuffer_info.renderPass = m_render_passes[r].m_render_pass;
				framebuffer_info.attachmentCount = attachments_length;
				framebuffer_info.pAttachments = attachments;
				framebuffer_info.width = getRenderWidth();
				framebuffer_info.height = getRenderHeight();
				framebuffer_info.layers = 1;

				if (vkCreateFramebuffer(m_device->getDevice(), &framebuffer_info, nullptr, &m_render_passes[r].m_framebuffers[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("Failed to create framebuffer!");
				}
			}
		}
	}

	void PixelSwapChain::createColorResources()
	{
		m_swap_chain_color_format = getPixelSwapChainImageFormat();
		for (unsigned int i = 0; i < m_image_count; i++)
		{
			VkImageCreateInfo image_info{};
			image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_info.imageType = VK_IMAGE_TYPE_2D;
			image_info.extent.width = getRenderWidth();
			image_info.extent.height = getRenderHeight();
			image_info.extent.depth = 1;
			image_info.mipLevels = 1;
			image_info.arrayLayers = 1;
			image_info.format = m_swap_chain_color_format;
			image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
			image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			image_info.samples = VK_SAMPLE_COUNT_1_BIT;
			image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			image_info.flags = 0;

			m_device->createImageWithInfo(
				&image_info,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&m_resource_set[i].m_color_image,
				&m_resource_set[i].m_color_image_memory
			);

			VkImageViewCreateInfo view_info{};
			view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view_info.image = m_resource_set[i].m_color_image;
			view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_info.format = m_swap_chain_color_format;
			view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			view_info.subresourceRange.baseMipLevel = 0;
			view_info.subresourceRange.levelCount = 1;
			view_info.subresourceRange.baseArrayLayer = 0;
			view_info.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device->getDevice(), &view_info, nullptr, &m_resource_set[i].m_color_image_view) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create texture image view!");
			}
		}
	}

	void PixelSwapChain::createDepthResources()
	{
		m_swap_chain_depth_format = findDepthFormat();
		for (unsigned int i = 0; i < m_image_count; i++)
		{
			VkImageCreateInfo image_info{};
			image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_info.imageType = VK_IMAGE_TYPE_2D;
			image_info.extent.width = getRenderWidth();
			image_info.extent.height = getRenderHeight();
			image_info.extent.depth = 1;
			image_info.mipLevels = 1;
			image_info.arrayLayers = 1;
			image_info.format = m_swap_chain_depth_format;
			image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
			image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			image_info.samples = VK_SAMPLE_COUNT_1_BIT;
			image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			image_info.flags = 0;

			m_device->createImageWithInfo(
				&image_info,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&m_resource_set[i].m_depth_image,
				&m_resource_set[i].m_depth_image_memory
			);

			VkImageViewCreateInfo view_info{};
			view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view_info.image = m_resource_set[i].m_depth_image;
			view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_info.format = m_swap_chain_depth_format;
			view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			view_info.subresourceRange.baseMipLevel = 0;
			view_info.subresourceRange.levelCount = 1;
			view_info.subresourceRange.baseArrayLayer = 0;
			view_info.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device->getDevice(), &view_info, nullptr, &m_resource_set[i].m_depth_image_view) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create texture image view!");
			}
		}
	}

	void PixelSwapChain::createSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (unsigned int i = 0; i < m_image_count; i++)
		{
			if (vkCreateSemaphore(m_device->getDevice(), &semaphoreInfo, nullptr, &m_resource_set[i].m_image_available_semaphore) != VK_SUCCESS ||
				vkCreateSemaphore(m_device->getDevice(), &semaphoreInfo, nullptr, &m_resource_set[i].m_render_finished_semaphore) != VK_SUCCESS ||
				vkCreateFence(m_device->getDevice(), &fenceInfo, nullptr, &m_resource_set[i].m_in_flight_fence) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create synchronization objects for a frame!");
			}
		}
	}

	void PixelSwapChain::createColorIntermediates()
	{
		for (unsigned int i = 0; i < m_image_count; i++)
		{
			VkImageCreateInfo image_info{};
			image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_info.imageType = VK_IMAGE_TYPE_2D;
			image_info.extent.width = getRenderWidth();
			image_info.extent.height = getRenderHeight();
			image_info.extent.depth = 1;
			image_info.mipLevels = 1;
			image_info.arrayLayers = 1;
			image_info.format = m_swap_chain_color_format;
			image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
			image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			image_info.samples = VK_SAMPLE_COUNT_1_BIT;
			image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			image_info.flags = 0;

			m_device->createImageWithInfo(
				&image_info,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&m_resource_set[i].m_color_image_intermediate,
				&m_resource_set[i].m_color_image_memory_intermediate
			);

			VkImageViewCreateInfo view_info{};
			view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view_info.image = m_resource_set[i].m_color_image_intermediate;
			view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_info.format = m_swap_chain_color_format;
			view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			view_info.subresourceRange.baseMipLevel = 0;
			view_info.subresourceRange.levelCount = 1;
			view_info.subresourceRange.baseArrayLayer = 0;
			view_info.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device->getDevice(), &view_info, nullptr, &m_resource_set[i].m_color_image_view_intermediate) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create texture image view!");
			}
		}

		// Transition swap chain images to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL because render pipeline expects it
		for (unsigned int i = 0; i < m_image_count; i++)
		{
			m_device->transitionImageLayout(
				m_resource_set[i].m_color_image_intermediate,
				m_swap_chain_image_format,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				1,
				1
			);
		}

		// samplers
		VkSamplerCreateInfo sampler_info{};
		sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_info.magFilter = VK_FILTER_NEAREST;
		sampler_info.minFilter = VK_FILTER_NEAREST;
		sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler_info.anisotropyEnable = VK_FALSE;
		sampler_info.maxAnisotropy = 1.0f;
		sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.compareEnable = VK_FALSE;
		sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
		sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		sampler_info.mipLodBias = 0.0f;
		sampler_info.minLod = 0.0f;
		sampler_info.maxLod = 0.0f;

		for (unsigned int i = 0; i < m_image_count; i++)
		{
			if (vkCreateSampler(m_device->getDevice(), &sampler_info, nullptr, &m_resource_set[i].m_color_sampler_intermediate) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create samplers!");
			}
		}

		// descriptors
		for (unsigned int i = 0; i < m_image_count; i++)
		{
			m_resource_set[i].m_color_descriptor_intermediate.sampler = m_resource_set[i].m_color_sampler_intermediate;
			m_resource_set[i].m_color_descriptor_intermediate.imageView = m_resource_set[i].m_color_image_view_intermediate;
			m_resource_set[i].m_color_descriptor_intermediate.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
	}

	void PixelSwapChain::createDepthIntermediates()
	{
		for (unsigned int i = 0; i < m_image_count; i++)
		{
			VkImageCreateInfo image_info{};
			image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_info.imageType = VK_IMAGE_TYPE_2D;
			image_info.extent.width = getRenderWidth();
			image_info.extent.height = getRenderHeight();
			image_info.extent.depth = 1;
			image_info.mipLevels = 1;
			image_info.arrayLayers = 1;
			image_info.format = m_swap_chain_depth_format;
			image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
			image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			image_info.samples = VK_SAMPLE_COUNT_1_BIT;
			image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			image_info.flags = 0;

			m_device->createImageWithInfo(
				&image_info,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&m_resource_set[i].m_depth_image_intermediate,
				&m_resource_set[i].m_depth_image_memory_intermediate
			);

			VkImageViewCreateInfo view_info{};
			view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view_info.image = m_resource_set[i].m_depth_image_intermediate;
			view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_info.format = m_swap_chain_depth_format;
			view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			view_info.subresourceRange.baseMipLevel = 0;
			view_info.subresourceRange.levelCount = 1;
			view_info.subresourceRange.baseArrayLayer = 0;
			view_info.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_device->getDevice(), &view_info, nullptr, &m_resource_set[i].m_depth_image_view_intermediate) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create texture image view!");
			}
		}

		// Transition swap chain images to VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL because render pipeline expects it
		for (unsigned int i = 0; i < m_image_count; i++)
		{
			m_device->transitionImageLayout(
				m_resource_set[i].m_depth_image_intermediate,
				m_swap_chain_depth_format,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
				1,
				1
			);
		}

		// samplers
		VkSamplerCreateInfo sampler_info{};
		sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_info.magFilter = VK_FILTER_NEAREST;
		sampler_info.minFilter = VK_FILTER_NEAREST;
		sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler_info.anisotropyEnable = VK_FALSE;
		sampler_info.maxAnisotropy = 1.0f;
		sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.compareEnable = VK_FALSE;
		sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
		sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		sampler_info.mipLodBias = 0.0f;
		sampler_info.minLod = 0.0f;
		sampler_info.maxLod = 0.0f;

		for (unsigned int i = 0; i < m_image_count; i++)
		{
			if (vkCreateSampler(m_device->getDevice(), &sampler_info, nullptr, &m_resource_set[i].m_depth_sampler_intermediate) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create samplers!");
			}
		}

		// descriptors
		for (unsigned int i = 0; i < m_image_count; i++)
		{
			m_resource_set[i].m_depth_descriptor_intermediate.sampler = m_resource_set[i].m_depth_sampler_intermediate;
			m_resource_set[i].m_depth_descriptor_intermediate.imageView = m_resource_set[i].m_depth_image_view_intermediate;
			m_resource_set[i].m_depth_descriptor_intermediate.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
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
		for (unsigned int i = 0; i < available_present_modes_count; i++)
		{
			if (available_present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				std::cout << "Present mode: Immediate" << '\n';
				return available_present_modes[i];
			}
		}
		for (unsigned int i = 0; i < available_present_modes_count; i++)
		{
			if (available_present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				std::cout << "Present mode: Mailbox" << '\n';
				return available_present_modes[i];
			}
		}
		std::cout << "Present mode: V-Sync" << '\n';
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
