// internal
#include "Pipeline.h"

// external
#include <stdexcept>

namespace Isonia::Pipeline
{
	Renderer::Renderer(Window* window, Device* device) : m_window(window), m_device(device)
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	Renderer::~Renderer()
	{
		freeCommandBuffers();
		m_swap_chain->freeOldSwapChain();
		delete m_swap_chain;
	}

	void Renderer::registerRenderResizeCallback(EventHandler handler, void* user_data)
	{
		m_handlers[m_event_count] = handler;
		m_user_data[m_event_count] = user_data;
		m_event_count++;
	}

	void Renderer::propigateRenderResizeEvent()
	{
		for (unsigned int i = 0; i < m_event_count; i++)
		{
			m_handlers[i](this, m_user_data[i]);
		}
	}

	VkRenderPass Renderer::getSwapChainRenderPass() const
	{
		return m_swap_chain->getRenderPass();
	}
	float Renderer::getAspectRatio() const
	{
		return m_swap_chain->getExtentAspectRatio();
	}
	VkExtent2D Renderer::getExtent() const
	{
		return m_swap_chain->getSwapChainExtent();
	}
	bool Renderer::isFrameInProgress() const
	{
		return m_is_frame_started;
	}

	VkCommandBuffer Renderer::getCurrentCommandBuffer() const
	{
		assert(m_is_frame_started && "Cannot get command buffer when frame not in progress");
		return m_command_buffers[m_current_frame];
	}

	int Renderer::getFrameIndex() const
	{
		assert(m_is_frame_started && "Cannot get frame index when frame not in progress");
		return m_current_frame;
	}

	VkCommandBuffer Renderer::beginFrame()
	{
		assert(!m_is_frame_started && "Can't call beginFrame while already in progress");

		VkResult result = m_swap_chain->acquireNextImage(&m_current_frame);
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

	void Renderer::endFrame()
	{
		assert(m_is_frame_started && "Can't call endFrame while frame is not in progress");
		VkCommandBuffer command_buffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer!");
		}

		VkResult result = m_swap_chain->submitCommandBuffers(&command_buffer, &m_current_frame);
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
		m_current_frame = (m_current_frame + 1) % m_swap_chain->getImageCount();
	}

	void Renderer::beginSwapChainRenderPass(VkCommandBuffer command_buffer)
	{
		assert(m_is_frame_started && "Can't call beginSwapChainRenderPass if frame is not in progress");
		assert(command_buffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo render_pass_info{};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = m_swap_chain->getRenderPass();
		render_pass_info.framebuffer = m_swap_chain->getFrameBuffer(m_current_frame);

		render_pass_info.renderArea.offset = { 0, 0 };
		render_pass_info.renderArea.extent = m_swap_chain->getSwapChainExtent();

		const unsigned int clear_values_count = 2;
		VkClearValue clear_values[clear_values_count]
		{
			{.color = { 0.01f, 0.01f, 0.01f, 1.0f }},
			{.depthStencil = { 1.0f, 0 }}
		};
		render_pass_info.clearValueCount = clear_values_count;
		render_pass_info.pClearValues = clear_values;

		vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_swap_chain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(m_swap_chain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, m_swap_chain->getSwapChainExtent() };
		vkCmdSetViewport(command_buffer, 0, 1, &viewport);
		vkCmdSetScissor(command_buffer, 0, 1, &scissor);
	}

	void Renderer::endSwapChainRenderPass(VkCommandBuffer command_buffer)
	{
		assert(m_is_frame_started && "Can't call endSwapChainRenderPass if frame is not in progress");
		assert(command_buffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(command_buffer);
	}

	void Renderer::createCommandBuffers()
	{
		VkCommandBufferAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = m_device->getCommandPool();
		alloc_info.commandBufferCount = m_swap_chain->getImageCount();

		if (vkAllocateCommandBuffers(m_device->getDevice(), &alloc_info, m_command_buffers) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers!");
		}
	}

	void Renderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			m_device->getDevice(),
			m_device->getCommandPool(),
			m_swap_chain->getImageCount(),
			m_command_buffers
		);
	}

	void Renderer::recreateSwapChain()
	{
		VkExtent2D extent = m_window->m_extent;
		while (extent.width == 0 || extent.height == 0)
		{
			m_window->pollEvents();
			extent = m_window->m_extent;
		}
		vkDeviceWaitIdle(m_device->getDevice());

		if (m_swap_chain == nullptr)
		{
			m_swap_chain = new SwapChain(m_device, extent);
		}
		else
		{
			SwapChain* old_swap_chain = m_swap_chain;
			old_swap_chain->freeOldSwapChain();
			m_swap_chain = new SwapChain(m_device, extent, old_swap_chain);

			if (!old_swap_chain->compareSwapFormats(m_swap_chain))
			{
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}
		}
	}
}
