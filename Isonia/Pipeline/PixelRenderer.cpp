// internal
#include "Pipeline.h"

// external
#include <stdexcept>

namespace Isonia::Pipeline
{
	PixelRenderer::PixelRenderer(Window* window, Device* device)
		: m_window(window), m_device(device)
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	PixelRenderer::~PixelRenderer()
	{
		freeCommandBuffers();
		m_pixel_swap_chain->freeOldPixelSwapChain();
		delete m_pixel_swap_chain;
	}

	void PixelRenderer::registerRenderResizeCallback(EventHandler handler, void* user_data)
	{
		m_handlers[m_event_count] = handler;
		m_user_data[m_event_count] = user_data;
		m_event_count++;
	}

	void PixelRenderer::propigateRenderResizeEvent()
	{
		for (unsigned int i = 0; i < m_event_count; i++)
		{
			m_handlers[i](this, m_user_data[i]);
		}
	}

	VkRenderPass PixelRenderer::getSwapChainRenderPass(const unsigned int index) const
	{
		return m_pixel_swap_chain->getRenderPass(index);
	}
	float PixelRenderer::getAspectRatio() const
	{
		return m_pixel_swap_chain->getExtentAspectRatio();
	}
	VkExtent2D PixelRenderer::getExtent() const
	{
		return m_pixel_swap_chain->getRenderExtent();
	}
	bool PixelRenderer::isFrameInProgress() const
	{
		return m_is_frame_started;
	}
	PixelSwapChain* PixelRenderer::getPixelSwapChain() const
	{
		return m_pixel_swap_chain;
	}

	VkCommandBuffer PixelRenderer::getCurrentCommandBuffer() const
	{
		assert(m_is_frame_started && "Cannot get command buffer when frame not in progress");
		return m_command_buffers[m_current_frame_index];
	}

	int PixelRenderer::getFrameIndex() const
	{
		assert(m_is_frame_started && "Cannot get frame index when frame not in progress");
		return m_current_frame_index;
	}

	unsigned int PixelRenderer::getRenderFactor() const
	{
		return m_render_factor;
	}

	VkCommandBuffer PixelRenderer::beginFrame()
	{
		assert(!m_is_frame_started && "Can't call beginFrame while already in progress");

		VkResult result = m_pixel_swap_chain->acquireNextImage(&m_current_image_index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		m_is_frame_started = true;

		VkCommandBuffer command_buffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}
		return command_buffer;
	}

	void PixelRenderer::endFrame()
	{
		assert(m_is_frame_started && "Can't call endFrame while frame is not in progress");
		VkCommandBuffer command_buffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer!");
		}

		VkResult result = m_pixel_swap_chain->submitCommandBuffers(&command_buffer, &m_current_image_index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window->m_resized)
		{
			m_window->m_resized = false;
			recreateSwapChain();
			propigateRenderResizeEvent();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image!");
		}

		m_is_frame_started = false;
		m_current_frame_index = (m_current_frame_index + 1) % PixelSwapChain::max_frames_in_flight;
	}

	void PixelRenderer::beginSwapChainRenderPass(VkCommandBuffer command_buffer, unsigned int render_pass_index)
	{
		assert(m_is_frame_started && "Can't call begin SwapChainRenderPass if frame is not in progress");
		assert(command_buffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

		m_pixel_swap_chain->m_current_render_pass_index = render_pass_index;

		VkRenderPassBeginInfo render_pass_info{};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = m_pixel_swap_chain->getRenderPass(render_pass_index);
		render_pass_info.framebuffer = m_pixel_swap_chain->getFrameBuffer(m_current_image_index);

		const VkExtent2D extent = m_pixel_swap_chain->getRenderExtent();
		const VkOffset2D offset = { 0, 0 };

		render_pass_info.renderArea.offset = offset;
		render_pass_info.renderArea.extent = extent;

		const unsigned int clear_values_count = 2;
		VkClearValue clear_values[clear_values_count]
		{
			{.color = { 0.00f, 0.00f, 0.00f, 1.0f }},
			{.depthStencil = { 1.0f, 0 }}
		};
		render_pass_info.clearValueCount = clear_values_count;
		render_pass_info.pClearValues = clear_values;

		vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(extent.width),
			.height = static_cast<float>(extent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};
		VkRect2D scissor{ offset, extent };
		vkCmdSetViewport(command_buffer, 0, 1, &viewport);
		vkCmdSetScissor(command_buffer, 0, 1, &scissor);
	}

	void PixelRenderer::endSwapChainRenderPass(VkCommandBuffer command_buffer)
	{
		assert(m_is_frame_started && "Can't call endPixelSwapChainRenderPass if frame is not in progress");
		assert(command_buffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(command_buffer);
	}

	void PixelRenderer::blit(VkCommandBuffer command_buffer, Math::Vector2 offset)
	{
		// The common subresource thingies
		VkImageSubresourceRange subresource_range{
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
		VkImageMemoryBarrier clear_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = m_pixel_swap_chain->getSwapChainImage(m_current_image_index),
			.subresourceRange = subresource_range
		};

		vkCmdPipelineBarrier(
			command_buffer,
			VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &clear_barrier
		);

		// Calculate pixel offsets
		const float scale_factor = Math::pixels_per_unit * static_cast<float>(m_render_factor);
		const int offset_x = Math::roundf_i(offset.x * scale_factor);
		const int offset_y = Math::roundf_i(offset.y * scale_factor);

		// Define source and destination offsets for image blit
		const int render_factor = static_cast<int>(m_render_factor);
		const int src_width = static_cast<int>(m_pixel_swap_chain->getRenderWidth());
		const int src_height = static_cast<int>(m_pixel_swap_chain->getRenderHeight());
		const int dst_width = src_width * render_factor;
		const int dst_height = src_height * render_factor;

		// Create image blit configuration
		VkImageBlit imageBlit
		{
			.srcSubresource = subresource,
			.srcOffsets = {
				{ 2, 2, 0 },
				{ src_width - 2, src_height - 2, 1 }
			},
			.dstSubresource = subresource,
			.dstOffsets = {
				{ render_factor + offset_x, render_factor + offset_y, 0 },
				{ dst_width - render_factor * 3 + offset_x, dst_height - render_factor * 3 + offset_y, 1 }
			}
		};

		// Blit the image to the swapchain image
		vkCmdBlitImage(
			command_buffer,
			m_pixel_swap_chain->getImage(m_current_image_index),
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			m_pixel_swap_chain->getSwapChainImage(m_current_image_index),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &imageBlit, VK_FILTER_NEAREST
		);

		// "Blit" the remaining renderFactor * 2 border using compute shader bc of hardware limitations of blit

		// Transfer to the presentation layout
		VkImageMemoryBarrier present_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = m_pixel_swap_chain->getSwapChainImage(m_current_image_index),
			.subresourceRange = subresource_range
		};

		vkCmdPipelineBarrier(
			command_buffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0, 0, nullptr, 0, nullptr, 1, &present_barrier
		);
	}

	void PixelRenderer::copyToIntermediates(VkCommandBuffer command_buffer)
	{
		// The common subresource thingies for color
		VkImageSubresourceRange color_subresource_range{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		};

		VkImageSubresourceLayers color_subresource{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1
		};

		// The common subresource thingies for depth
		VkImageSubresourceRange depth_subresource_range{
			.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		};

		VkImageSubresourceLayers depth_subresource{
			.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 1
		};

		// Transfer the intermediate color image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, so we can copy to it
		VkImageMemoryBarrier clear_color_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = m_pixel_swap_chain->getIntermediateImage(m_current_image_index),
			.subresourceRange = color_subresource_range
		};

		// Transfer the intermediate depth image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, so we can copy to it
		VkImageMemoryBarrier clear_depth_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = m_pixel_swap_chain->getIntermediateDepthImage(m_current_image_index),
			.subresourceRange = depth_subresource_range
		};

		// Pipeline barriers for color and depth
		vkCmdPipelineBarrier(
			command_buffer,
			VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &clear_color_barrier
		);

		vkCmdPipelineBarrier(
			command_buffer,
			VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &clear_depth_barrier
		);

		// Set up the copy region for color
		VkImageCopy color_copy_region{
			.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
			.srcOffset = { 0, 0, 0 },
			.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
			.dstOffset = { 0, 0, 0 },
			.extent = {
				m_pixel_swap_chain->getRenderWidth(),
				m_pixel_swap_chain->getRenderHeight(),
				1
			}
		};

		// Set up the copy region for depth
		VkImageCopy depth_copy_region{
			.srcSubresource = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 0, 1 },
			.srcOffset = { 0, 0, 0 },
			.dstSubresource = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 0, 1 },
			.dstOffset = { 0, 0, 0 },
			.extent = {
				m_pixel_swap_chain->getRenderWidth(),
				m_pixel_swap_chain->getRenderHeight(),
				1
			}
		};

		// Perform the copy operation for color
		vkCmdCopyImage(
			command_buffer,
			m_pixel_swap_chain->getImage(m_current_image_index),
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			m_pixel_swap_chain->getIntermediateImage(m_current_image_index),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &color_copy_region
		);

		// Perform the copy operation for depth
		vkCmdCopyImage(
			command_buffer,
			m_pixel_swap_chain->getDepthImage(m_current_image_index),
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			m_pixel_swap_chain->getIntermediateDepthImage(m_current_image_index),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &depth_copy_region
		);

		// Transfer the intermediate color image to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		VkImageMemoryBarrier present_color_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = m_pixel_swap_chain->getIntermediateImage(m_current_image_index),
			.subresourceRange = color_subresource_range
		};

		// Transfer the intermediate depth image to VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
		VkImageMemoryBarrier present_depth_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = m_pixel_swap_chain->getIntermediateDepthImage(m_current_image_index),
			.subresourceRange = depth_subresource_range
		};

		// Pipeline barriers for color and depth to presentation layout
		vkCmdPipelineBarrier(
			command_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &present_color_barrier
		);

		vkCmdPipelineBarrier(
			command_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &present_depth_barrier
		);
	}

	void PixelRenderer::createCommandBuffers()
	{
		VkCommandBufferAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = m_device->getCommandPool();
		alloc_info.commandBufferCount = PixelSwapChain::max_frames_in_flight;

		if (vkAllocateCommandBuffers(m_device->getDevice(), &alloc_info, m_command_buffers) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers!");
		}
	}

	void PixelRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			m_device->getDevice(),
			m_device->getCommandPool(),
			PixelSwapChain::max_frames_in_flight,
			m_command_buffers
		);
	}

	void PixelRenderer::calculateResolution(VkExtent2D window_extent, float* out_width, float* out_height, unsigned int* out_render_factor)
	{
		static const constexpr float ideal_pixel_density = 640.0f * 360.0f; //512.0f * 288.0f;

		float closest_ideal_pixel_density = Math::float_max;
		for (unsigned int factor = 1; factor <= 8; factor++)
		{
			const float width = static_cast<float>(window_extent.width) / static_cast<float>(factor);
			const float height = static_cast<float>(window_extent.height) / static_cast<float>(factor);
			const float pixel_density = width * height;
			const float difference = Math::absf(ideal_pixel_density - pixel_density);

			if (difference < Math::absf(ideal_pixel_density - closest_ideal_pixel_density))
			{
				closest_ideal_pixel_density = pixel_density;
				*out_width = width;
				*out_height = height;

				*out_render_factor = factor;
			}
		}
	}

	VkExtent2D PixelRenderer::recalculateCameraSettings(VkExtent2D window_extent, unsigned int* out_render_factor)
	{
		float render_width; float render_height;
		calculateResolution(window_extent, &render_width, &render_height, out_render_factor);

		// extended so we can render sub-pixels
		//return {
		//	static_cast<unsigned int>(Math::floorf_i(render_width)),
		//	static_cast<unsigned int>(Math::floorf_i(render_height))
		//};
		return {
			static_cast<unsigned int>(Math::floorf_i(render_width)) + 2,
			static_cast<unsigned int>(Math::floorf_i(render_height)) + 2
		};
		// odd number so it snaps as little as posible on camera rotation extended so we can render sub-pixels
		return {
			static_cast<unsigned int>(Math::ceilOddf_i(render_width)) + 2,
			static_cast<unsigned int>(Math::ceilOddf_i(render_height)) + 2
		};
	}

	void PixelRenderer::recreateSwapChain()
	{
		VkExtent2D window_extent = m_window->m_extent;
		while (window_extent.width == 0 || window_extent.height == 0)
		{
			m_window->pollEvents();
			window_extent = m_window->m_extent;
		}
		vkDeviceWaitIdle(m_device->getDevice());

		VkExtent2D render_extent = recalculateCameraSettings(window_extent, &m_render_factor);

		if (m_pixel_swap_chain == nullptr)
		{
			m_pixel_swap_chain = new PixelSwapChain(m_device, window_extent, render_extent);
		}
		else
		{
			PixelSwapChain* old_pixel_swap_chain = m_pixel_swap_chain;
			old_pixel_swap_chain->freeOldPixelSwapChain();
			m_pixel_swap_chain = new PixelSwapChain(m_device, window_extent, render_extent, old_pixel_swap_chain);

			if (!old_pixel_swap_chain->compareSwapFormats(m_pixel_swap_chain))
			{
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}
		}
	}
}
