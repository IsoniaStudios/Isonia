// internal
#include "Pipeline.h"

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

	void PixelRenderer::registerRenderResizeCallback(EventHandler handler)
	{
		m_handlers.push_back(handler);
	}

	void PixelRenderer::propigateRenderResizeEvent()
	{
		for (const auto& handler : m_handlers)
		{
			handler(this);
		}
	}

	VkRenderPass PixelRenderer::getSwapChainRenderPass() const
	{
		return m_pixel_swap_chain->getRenderPass();
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

	VkCommandBuffer PixelRenderer::beginFrame()
	{
		assert(!m_is_frame_started && "Can't call beginFrame while already in progress");

		auto result = m_pixel_swap_chain->acquireNextImage(&m_current_image_index);
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

		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}
		return commandBuffer;
	}

	void PixelRenderer::endFrame()
	{
		assert(m_is_frame_started && "Can't call endFrame while frame is not in progress");
		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer!");
		}

		auto result = m_pixel_swap_chain->submitCommandBuffers(&commandBuffer, &m_current_image_index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window->wasWindowResized())
		{
			m_window->resetWindowResizedFlag();
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

	void PixelRenderer::beginSwapChainRenderPass(VkCommandBuffer command_buffer)
	{
		assert(m_is_frame_started && "Can't call begin SwapChainRenderPass if frame is not in progress");
		assert(command_buffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_pixel_swap_chain->getRenderPass();
		renderPassInfo.framebuffer = m_pixel_swap_chain->getFrameBuffer(m_current_image_index);

		const VkExtent2D extent = m_pixel_swap_chain->getRenderExtent();
		const VkOffset2D offset = { 0, 0 };

		renderPassInfo.renderArea.offset = offset;
		renderPassInfo.renderArea.extent = extent;

		const unsigned int clearValuesCount = 2;
		VkClearValue clearValues[clearValuesCount]
		{
			{.color = { 0.01f, 0.01f, 0.01f, 1.0f }},
			{.depthStencil = { 1.0f, 0 }}
		};
		renderPassInfo.clearValueCount = clearValuesCount;
		renderPassInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

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
			.image = m_pixel_swap_chain->getSwapChainImage(m_current_image_index),
			.subresourceRange = subresourceRange
		};

		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &clearBarrier);

		// Calculate pixel offsets
		const float scaleFactor = Math::pixels_per_unit * static_cast<float>(Math::render_factor);
		const int offsetX = Math::roundf_i(offset.x * scaleFactor);
		const int offsetY = Math::roundf_i(offset.y * scaleFactor);

		// Define source and destination offsets for image blit
		const int render_factor = static_cast<int>(Math::render_factor);
		const int srcWidth = static_cast<int>(m_pixel_swap_chain->getRenderWidth());
		const int srcHeight = static_cast<int>(m_pixel_swap_chain->getRenderHeight());
		const int dstWidth = srcWidth * render_factor;
		const int dstHeight = srcHeight * render_factor;

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
				{ render_factor + offsetX, render_factor + offsetY, 0 },
				{ dstWidth - render_factor * 3 + offsetX, dstHeight - render_factor * 3 + offsetY, 1 }
			}
		};

		// Blit the image to the swapchain image
		vkCmdBlitImage(command_buffer, m_pixel_swap_chain->getImage(m_current_image_index), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_pixel_swap_chain->getSwapChainImage(m_current_image_index), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlit, VK_FILTER_NEAREST);

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
			.image = m_pixel_swap_chain->getSwapChainImage(m_current_image_index),
			.subresourceRange = subresourceRange
		};

		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &presentBarrier);
	}

	void PixelRenderer::createCommandBuffers()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_device->getCommandPool();
		allocInfo.commandBufferCount = PixelSwapChain::max_frames_in_flight;

		if (vkAllocateCommandBuffers(m_device->getDevice(), &allocInfo, m_command_buffers) != VK_SUCCESS)
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

	void PixelRenderer::calculateResolution(VkExtent2D window_extent, float* out_width, float* out_height)
	{
		static const constexpr float idealPixelDensity = 640.0f * 360.0f; //512.0f * 288.0f;

		float closestIdealPixelDensity = std::numeric_limits<float>::max();
		for (unsigned int factor = 1; factor <= 8; factor++)
		{
			const float width = static_cast<float>(window_extent.width) / static_cast<float>(factor);
			const float height = static_cast<float>(window_extent.height) / static_cast<float>(factor);
			const float pixelDensity = width * height;
			const float difference = std::abs(idealPixelDensity - pixelDensity);

			if (difference < std::abs(idealPixelDensity - closestIdealPixelDensity))
			{
				closestIdealPixelDensity = pixelDensity;
				*out_width = width;
				*out_height = height;

				Math::render_factor = factor;
			}
		}
	}

	VkExtent2D PixelRenderer::recalculateCameraSettings(VkExtent2D window_extent)
	{
		float render_width; float render_height;
		calculateResolution(window_extent, &render_width, &render_height);

		// extended so we can render sub-pixels
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
		VkExtent2D window_extent = m_window->getExtent();
		while (window_extent.width == 0 || window_extent.height == 0)
		{
			window_extent = m_window->getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(m_device->getDevice());

		VkExtent2D render_extent = recalculateCameraSettings(window_extent);

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
