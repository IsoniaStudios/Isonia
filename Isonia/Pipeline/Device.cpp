// internal
#include "Pipeline.h"

// external
#include <cstring>
#include <stdexcept>
#include <iostream>

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

	VkFormat Device::findSupportedFormat(const VkFormat* candidates, const unsigned int candidates_count, VkImageTiling tiling, VkFormatFeatureFlags features) const
	{
		for (unsigned int i = 0; i < candidates_count; i++)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_physical_device, candidates[i], &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return candidates[i];
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				return candidates[i];
			}
		}
		throw std::runtime_error("Failed to find supported format!");
	}

	unsigned int Device::findMemoryType(unsigned int type_filter, VkMemoryPropertyFlags properties) const
	{
		VkPhysicalDeviceMemoryProperties mem_properties;
		vkGetPhysicalDeviceMemoryProperties(m_physical_device, &mem_properties);
		for (unsigned int i = 0u; i < mem_properties.memoryTypeCount; i++)
		{
			if ((type_filter & (1u << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type!");
	}

	void Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* buffer_memory)
	{
		VkBufferCreateInfo buffer_info{};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = size;
		buffer_info.usage = usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(m_device, &buffer_info, nullptr, buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create vertex buffer!");
		}

		VkMemoryRequirements mem_requirements;
		vkGetBufferMemoryRequirements(m_device, *buffer, &mem_requirements);

		VkMemoryAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = mem_requirements.size;
		alloc_info.memoryTypeIndex = findMemoryType(mem_requirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_device, &alloc_info, nullptr, buffer_memory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(m_device, *buffer, *buffer_memory, 0);
	}

	VkCommandBuffer Device::beginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = m_command_pool;
		alloc_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(m_device, &alloc_info, &command_buffer);

		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(command_buffer, &begin_info);
		return command_buffer;
	}

	void Device::endSingleTimeCommands(VkCommandBuffer command_buffer)
	{
		vkEndCommandBuffer(command_buffer);

		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		vkQueueSubmit(m_graphics_queue, 1, &submit_info, nullptr);
		vkQueueWaitIdle(m_graphics_queue);

		vkFreeCommandBuffers(m_device, m_command_pool, 1, &command_buffer);
	}

	void Device::copyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
	{
		VkCommandBuffer command_buffer = beginSingleTimeCommands();

		VkBufferCopy copy_region{};
		copy_region.srcOffset = 0; // Optional
		copy_region.dstOffset = 0; // Optional
		copy_region.size = size;
		vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

		endSingleTimeCommands(command_buffer);
	}

	void Device::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout, unsigned int mip_levels, unsigned int layer_count)
	{
		// uses an image memory barrier transition image layouts and transfer queue
		// family ownership when VK_SHARING_MODE_EXCLUSIVE is used. There is an
		// equivalent buffer memory barrier to do this for buffers
		VkCommandBuffer command_buffer = beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = old_layout;
		barrier.newLayout = new_layout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mip_levels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layer_count;

		if (format == VK_FORMAT_D32_SFLOAT || new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
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

		VkPipelineStageFlags source_stage;
		VkPipelineStageFlags destination_stage;

		if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destination_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}
		else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			throw std::invalid_argument("Unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			command_buffer,
			source_stage,
			destination_stage,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier
		);

		endSingleTimeCommands(command_buffer);
	}

	void Device::copyBufferToImage(VkBuffer buffer, VkImage image, unsigned int width, unsigned int height, unsigned int layer_count)
	{
		VkCommandBuffer command_buffer = beginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layer_count;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(
			command_buffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		endSingleTimeCommands(command_buffer);
	}

	void Device::createImageWithInfo(const VkImageCreateInfo* image_info, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* image_memory)
	{
		if (vkCreateImage(m_device, image_info, nullptr, image) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image!");
		}

		VkMemoryRequirements mem_requirements;
		vkGetImageMemoryRequirements(m_device, *image, &mem_requirements);

		VkMemoryAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = mem_requirements.size;
		alloc_info.memoryTypeIndex = findMemoryType(mem_requirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_device, &alloc_info, nullptr, image_memory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate image memory!");
		}

		if (vkBindImageMemory(m_device, *image, *image_memory, 0) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to bind image memory!");
		}
	}

#ifdef DEBUG
	VkResult Device::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger)
	{
		PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, create_info, allocator, debug_messenger);
		}
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void Device::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator)
	{
		PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debug_messenger, allocator);
		}
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL Device::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
	{
		std::cerr << "validation layer: " << callback_data->pMessage << '\n';
		return VK_FALSE;
	}

	void Device::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* create_info)
	{
		create_info->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		create_info->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		create_info->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		create_info->pfnUserCallback = debugCallback;
		create_info->pUserData = nullptr;
	}

	void Device::setupDebugMessenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT create_info{};
		populateDebugMessengerCreateInfo(&create_info);
		if (createDebugUtilsMessengerEXT(m_instance, &create_info, nullptr, &m_debug_messenger) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to set up debug messenger!");
		}
	}

	bool Device::checkValidationLayerSupport()
	{
		unsigned int layer_count;
		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
		VkLayerProperties* available_layers = (VkLayerProperties*)malloc(layer_count * sizeof(VkLayerProperties));
		vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

		for (unsigned int i = 0; i < m_validation_layers_count; i++)
		{
			bool layer_found = false;
			const char* layer_name = m_validation_layers[i];

			for (unsigned int q = 0; q < layer_count; q++)
			{
				if (strcmp(layer_name, available_layers[q].layerName) == 0)
				{
					layer_found = true;
					break;
				}
			}

			if (!layer_found)
			{
				free(available_layers);
				return false;
			}
		}

		free(available_layers);
		return true;
	}
#endif

	const char** Device::getRequiredExtensions(unsigned int* count)
	{
#ifdef DEBUG
		*count = 3;
#else
		*count = 2;
#endif
		return new const char*[] {
			"VK_KHR_surface",
			"VK_KHR_win32_surface",
#ifdef DEBUG
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
		};
	}

	void Device::hasRequiredInstanceExtensions()
	{
		unsigned int extension_count = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
		VkExtensionProperties* extensions = (VkExtensionProperties*)malloc(extension_count * sizeof(VkExtensionProperties));
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions);

		std::cout << "available extensions:" << '\n';
		for (unsigned int i = 0; i < extension_count; i++)
		{
			std::cout << "\t" << extensions[i].extensionName << '\n';
		}

		std::cout << "required extensions:" << '\n';
		unsigned int required_extensions_count = 0;
		const char** required_extensions = getRequiredExtensions(&required_extensions_count);
		for (unsigned int i = 0; i < required_extensions_count; i++)
		{
			bool missing = true;
			std::cout << "\t" << required_extensions[i] << '\n';
			for (unsigned int q = 0; q < extension_count; q++)
			{
				if (strcmp(extensions[q].extensionName, required_extensions[i]) == 0)
				{
					missing = false;
				}
			}
			if (missing)
			{
				throw std::runtime_error("Missing required extension");
			}
		}

		free(extensions);
	}

	bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device)
	{
		unsigned int extension_count;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
		VkExtensionProperties* available_extensions = (VkExtensionProperties*)malloc(extension_count * sizeof(VkExtensionProperties));
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions);

		bool supported = true;
		for (unsigned int req_i = 0; req_i < m_device_extensions_count; req_i++)
		{
			bool found = false;
			for (unsigned int av_i = 0; av_i < extension_count; av_i++)
			{
				if (strcmp(m_device_extensions[req_i], available_extensions[av_i].extensionName) == 0)
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				supported = false;
				break;
			}
		}

		free(available_extensions);
		return supported;
	}

	QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device) const
	{
		QueueFamilyIndices indices;

		unsigned int queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
		VkQueueFamilyProperties* queue_families = (VkQueueFamilyProperties*)malloc(queue_family_count * sizeof(VkQueueFamilyProperties));
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

		for (unsigned int i = 0; i < queue_family_count; i++)
		{
			if (queue_families[i].queueCount > 0 && queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphics_family = i;
				indices.graphics_family_has_value = true;
			}
			VkBool32 present_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &present_support);
			if (queue_families[i].queueCount > 0 && present_support)
			{
				indices.present_family = i;
				indices.present_family_has_value = true;
			}
			if (indices.isComplete())
			{
				break;
			}
		}

		free(queue_families);
		return indices;
	}

	SwapChainSupportDetails Device::findSwapChainSupport(VkPhysicalDevice device) const
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &details.formats_count, nullptr);
		if (details.formats_count != 0)
		{
			details.formats = (VkSurfaceFormatKHR*)malloc(details.formats_count * sizeof(VkSurfaceFormatKHR));
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &details.formats_count, details.formats);
		}

		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &details.present_modes_count, nullptr);
		if (details.present_modes_count != 0)
		{
			details.present_modes = (VkPresentModeKHR*)malloc(details.present_modes_count * sizeof(VkPresentModeKHR));
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				device,
				m_surface,
				&details.present_modes_count,
				details.present_modes
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

		VkApplicationInfo app_info = {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pApplicationName = "Isonia";
		app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.pEngineName = "Isonia";
		app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info;

		unsigned int extensions_count;
		const char** extensions = getRequiredExtensions(&extensions_count);
		create_info.enabledExtensionCount = extensions_count;
		create_info.ppEnabledExtensionNames = extensions;

#ifdef DEBUG
		VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
		create_info.enabledLayerCount = m_validation_layers_count;
		create_info.ppEnabledLayerNames = m_validation_layers;

		populateDebugMessengerCreateInfo(&debug_create_info);
		create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
#else
		create_info.enabledLayerCount = 0;
		create_info.pNext = nullptr;
#endif

		if (vkCreateInstance(&create_info, nullptr, &m_instance) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create instance!");
		}

		hasRequiredInstanceExtensions();
	}

	void Device::pickPhysicalDevice()
	{
		unsigned int device_count = 0;
		vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);
		if (device_count == 0)
		{
			throw std::runtime_error("Failed to find GPUs with Vulkan support!");
		}
		std::cout << "Device count: " << device_count << '\n';
		VkPhysicalDevice* devices = (VkPhysicalDevice*)malloc(device_count * sizeof(VkPhysicalDevice));
		vkEnumeratePhysicalDevices(m_instance, &device_count, devices);

		for (unsigned int i = 0; i < device_count; i++)
		{
			if (isDeviceSuitable(devices[i]))
			{
				m_physical_device = devices[i];
				vkGetPhysicalDeviceProperties(m_physical_device, &m_properties);
				if (m_properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					break;
				}
			}
		}

		free(devices);

		if (m_physical_device == nullptr)
		{
			throw std::runtime_error("Failed to find a suitable GPU!");
		}

		std::cout << "Physical device: " << m_properties.deviceName << '\n';
	}

	void Device::createLogicalDevice()
	{
		QueueFamilyIndices indices = findQueueFamilies(m_physical_device);

		float queue_priority = 1.0f;
		unsigned int queue_create_infos_count;
		VkDeviceQueueCreateInfo* queue_create_infos;
		if (indices.graphics_family != indices.present_family)
		{
			queue_create_infos_count = 2;
			queue_create_infos = (VkDeviceQueueCreateInfo*)malloc(queue_create_infos_count * sizeof(VkDeviceQueueCreateInfo));
			// Queue families are distinct
			queue_create_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_infos[0].queueFamilyIndex = indices.graphics_family;
			queue_create_infos[0].queueCount = 1;
			queue_create_infos[0].pQueuePriorities = &queue_priority;
			queue_create_infos[0].pNext = nullptr;

			queue_create_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_infos[1].queueFamilyIndex = indices.present_family;
			queue_create_infos[1].queueCount = 1;
			queue_create_infos[1].pQueuePriorities = &queue_priority;
			queue_create_infos[1].pNext = nullptr;
		}
		else
		{
			queue_create_infos_count = 1;
			queue_create_infos = new VkDeviceQueueCreateInfo {
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				nullptr,
				0,
				indices.graphics_family,
				1,
				&queue_priority
			};
		}

		VkPhysicalDeviceFeatures device_features = {};
		device_features.geometryShader = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = queue_create_infos_count;
		createInfo.pQueueCreateInfos = queue_create_infos;

		createInfo.pEnabledFeatures = &device_features;
		createInfo.enabledExtensionCount = m_device_extensions_count;
		createInfo.ppEnabledExtensionNames = m_device_extensions;

#ifdef DEBUG
		// might not really be necessary anymore because device specific validation layers have been deprecated
		createInfo.enabledLayerCount = m_validation_layers_count;
		createInfo.ppEnabledLayerNames = m_validation_layers;
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
		QueueFamilyIndices queue_family_indices = getPhysicalQueueFamilies();

		VkCommandPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pool_info.queueFamilyIndex = queue_family_indices.graphics_family;
		pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(m_device, &pool_info, nullptr, &m_command_pool) != VK_SUCCESS)
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

		bool extensions_supported = checkDeviceExtensionSupport(device);

		bool swap_chain_adequate = false;
		if (extensions_supported)
		{
			SwapChainSupportDetails swap_chain_support = findSwapChainSupport(device);
			swap_chain_adequate = swap_chain_support.formats_count != 0u && swap_chain_support.present_modes_count != 0u;
		}

		VkPhysicalDeviceFeatures supported_features;
		vkGetPhysicalDeviceFeatures(device, &supported_features);

		return indices.isComplete() && extensions_supported && swap_chain_adequate && supported_features.geometryShader;
	}
}
