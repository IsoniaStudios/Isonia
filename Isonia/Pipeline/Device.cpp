// internal
#include "Pipeline.h"

namespace Isonia::Pipeline
{
	Device::Device(Window* window) : m_window(window)
	{
		createInstance();
#ifdef DEBUG
		setupDebugMessenger();
#endif
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createCommandPool();
	}

	Device::~Device()
	{
		vkDestroyCommandPool(m_device, m_command_pool, nullptr);
		vkDestroyDevice(m_device, nullptr);
#ifdef DEBUG
		destroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
#endif
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		vkDestroyInstance(m_instance, nullptr);
	}

	VkCommandPool Device::getCommandPool() const
	{
		return m_command_pool;
	}
	VkDevice Device::getDevice() const
	{
		return m_device;
	}
	VkSurfaceKHR Device::getSurface() const
	{
		return m_surface;
	}
	VkQueue Device::getGraphicsQueue() const
	{
		return m_graphics_queue;
	}
	VkQueue Device::getPresentQueue() const
	{
		return m_present_queue;
	}
	SwapChainSupportDetails Device::getSwapChainSupport()
	{
		return findSwapChainSupport(m_physical_device);
	}
	QueueFamilyIndices Device::getPhysicalQueueFamilies()
	{
		return findQueueFamilies(m_physical_device);
	}

	VkFormat Device::findSupportedFormat(const std::vector<VkFormat>* candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (VkFormat format : (*candidates))
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}
		throw std::runtime_error("Failed to find supported format!");
	}

	unsigned int Device::findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_physical_device, &memProperties);
		for (unsigned int i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type!");
	}

	void Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(m_device, &bufferInfo, nullptr, buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create vertex buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_device, *buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_device, &allocInfo, nullptr, bufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(m_device, *buffer, *bufferMemory, 0);
	}

	VkCommandBuffer Device::beginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_command_pool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		return commandBuffer;
	}

	void Device::endSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_graphics_queue);

		vkFreeCommandBuffers(m_device, m_command_pool, 1, &commandBuffer);
	}

	void Device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;  // Optional
		copyRegion.dstOffset = 0;  // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);
	}

	void Device::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, unsigned int mipLevels, unsigned int layerCount)
	{
		// uses an image memory barrier transition image layouts and transfer queue
		// family ownership when VK_SHARING_MODE_EXCLUSIVE is used. There is an
		// equivalent buffer memory barrier to do this for buffers
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layerCount;

		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT)
			{
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}
		else
		{
			throw std::invalid_argument("Unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage,
			destinationStage,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier
		);

		endSingleTimeCommands(commandBuffer);
	}

	void Device::copyBufferToImage(VkBuffer buffer, VkImage image, unsigned int width, unsigned int height, unsigned int layerCount)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layerCount;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		endSingleTimeCommands(commandBuffer);
	}

	void Device::createImageWithInfo(const VkImageCreateInfo* imageInfo, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory)
	{
		if (vkCreateImage(m_device, imageInfo, nullptr, image) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_device, *image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_device, &allocInfo, nullptr, imageMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate image memory!");
		}

		if (vkBindImageMemory(m_device, *image, *imageMemory, 0) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to bind image memory!");
		}
	}

#ifdef DEBUG
	VkResult Device::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void Device::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL Device::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	void Device::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo)
	{
		createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo->pfnUserCallback = debugCallback;
		createInfo->pUserData = nullptr;
	}

	void Device::setupDebugMessenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		populateDebugMessengerCreateInfo(&createInfo);
		if (createDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debug_messenger) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to set up debug messenger!");
		}
	}

	bool Device::checkValidationLayerSupport()
	{
		unsigned int layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		VkLayerProperties* availableLayers = new VkLayerProperties[layerCount];
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

		for (const char* layerName : m_validation_layers)
		{
			bool layerFound = false;

			for (unsigned int i = 0; i < layerCount; i++)
			{
				if (strcmp(layerName, availableLayers[i].layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				delete[] availableLayers;
				return false;
			}
		}

		delete[] availableLayers;
		return true;
	}
#endif

	std::vector<const char*> Device::getRequiredExtensions()
	{
		unsigned int glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifndef NDEBUG
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		return extensions;
	}

	void Device::hasGLFWRequiredInstanceExtensions()
	{
		unsigned int extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		VkExtensionProperties* extensions = new VkExtensionProperties[extensionCount];
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);

		std::cout << "available extensions:" << std::endl;
		std::unordered_set<std::string> available;
		for (unsigned int i = 0; i < extensionCount; i++)
		{
			std::cout << "\t" << extensions[i].extensionName << std::endl;
			available.insert(extensions[i].extensionName);
		}

		delete[] extensions;

		std::cout << "required extensions:" << std::endl;
		std::vector<const char*> requiredExtensions = getRequiredExtensions();
		for (const auto& required : requiredExtensions)
		{
			std::cout << "\t" << required << std::endl;
			if (available.find(required) == available.end())
			{
				throw std::runtime_error("Missing required glfw extension");
			}
		}
	}

	bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device)
	{
		unsigned int extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		VkExtensionProperties* availableExtensions = new VkExtensionProperties[extensionCount];
		vkEnumerateDeviceExtensionProperties(
			device,
			nullptr,
			&extensionCount,
			availableExtensions
		);

		std::set<std::string> requiredExtensions(m_device_extensions.begin(), m_device_extensions.end());

		for (unsigned int i = 0; i < extensionCount; i++)
		{
			requiredExtensions.erase(availableExtensions[i].extensionName);
		}

		delete[] availableExtensions;

		return requiredExtensions.empty();
	}

	QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;

		unsigned int queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		VkQueueFamilyProperties* queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

		for (unsigned int i = 0; i < queueFamilyCount; i++)
		{
			if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphics_family = i;
				indices.graphics_family_has_value = true;
			}
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
			if (queueFamilies[i].queueCount > 0 && presentSupport)
			{
				indices.present_family = i;
				indices.present_family_has_value = true;
			}
			if (indices.isComplete())
			{
				break;
			}
		}

		delete[] queueFamilies;

		return indices;
	}

	SwapChainSupportDetails Device::findSwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

		unsigned int formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
		}

		unsigned int presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.present_modes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				device,
				m_surface,
				&presentModeCount,
				details.present_modes.data()
			);
		}
		return details;
	}

	void Device::createInstance()
	{
#ifdef DEBUG
		if (!checkValidationLayerSupport())
		{
			throw std::runtime_error("Validation layers requested, but not available!");
		}
#endif

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Isonia";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Isonia";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		std::vector<const char*> extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<unsigned int>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef DEBUG
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		createInfo.enabledLayerCount = static_cast<unsigned int>(m_validation_layers.size());
		createInfo.ppEnabledLayerNames = m_validation_layers.data();

		populateDebugMessengerCreateInfo(&debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#else
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
#endif

		if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create instance!");
		}

		hasGLFWRequiredInstanceExtensions();
	}

	void Device::pickPhysicalDevice()
	{
		unsigned int deviceCount = 0;
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
		if (deviceCount == 0)
		{
			throw std::runtime_error("Failed to find GPUs with Vulkan support!");
		}
		std::cout << "Device count: " << deviceCount << std::endl;
		VkPhysicalDevice* devices = new VkPhysicalDevice[deviceCount];
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices);

		for (unsigned int i = 0; i < deviceCount; i++)
		{
			if (isDeviceSuitable(devices[i]))
			{
				m_physical_device = devices[i];
				break;
			}
		}

		delete[] devices;

		if (m_physical_device == VK_NULL_HANDLE)
		{
			throw std::runtime_error("Failed to find a suitable GPU!");
		}

		vkGetPhysicalDeviceProperties(m_physical_device, &m_properties);
		std::cout << "Physical device: " << m_properties.deviceName << std::endl;
	}

	void Device::createLogicalDevice()
	{
		QueueFamilyIndices indices = findQueueFamilies(m_physical_device);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<unsigned int> uniqueQueueFamilies = { indices.graphics_family, indices.present_family };

		float queuePriority = 1.0f;
		for (unsigned int queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_FALSE;
		deviceFeatures.geometryShader = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<unsigned int>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<unsigned int>(m_device_extensions.size());
		createInfo.ppEnabledExtensionNames = m_device_extensions.data();

#ifdef DEBUG
		// might not really be necessary anymore because device specific validation layers have been deprecated
		createInfo.enabledLayerCount = static_cast<unsigned int>(m_validation_layers.size());
		createInfo.ppEnabledLayerNames = m_validation_layers.data();
#else
		createInfo.enabledLayerCount = 0;
#endif

		if (vkCreateDevice(m_physical_device, &createInfo, nullptr, &m_device) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create logical device!");
		}

		vkGetDeviceQueue(m_device, indices.graphics_family, 0, &m_graphics_queue);
		vkGetDeviceQueue(m_device, indices.present_family, 0, &m_present_queue);
	}

	void Device::createCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = getPhysicalQueueFamilies();

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphics_family;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_command_pool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create command pool!");
		}
	}

	void Device::createSurface()
	{
		m_window->createWindowSurface(m_instance, &m_surface);
	}

	bool Device::isDeviceSuitable(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices = findQueueFamilies(device);

		bool extensionsSupported = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = findSwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.present_modes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.geometryShader;
	}
}
