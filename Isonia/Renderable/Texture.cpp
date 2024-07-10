// internal
#include "Renderable.h"

// external
#include <cstring>
#include <stdexcept>

namespace Isonia::Renderable
{
	Texture::Texture(Pipeline::Device* device, const void* texture, const unsigned int tex_height, const unsigned int tex_width, const VkFormat format, const VkFilter filter, const VkSamplerAddressMode address_mode)
		: m_device{ device }
	{
		createTextureImage(texture, tex_height, tex_width, format);
		createTextureImageView();
		createTextureSampler(filter, address_mode);
		updateDescriptor();
	}

	Texture::~Texture()
	{
		vkDestroySampler(m_device->getDevice(), m_texture_sampler, nullptr);
		vkDestroyImageView(m_device->getDevice(), m_texture_image_view, nullptr);
		vkDestroyImage(m_device->getDevice(), m_texture_image, nullptr);
		vkFreeMemory(m_device->getDevice(), m_texture_image_memory, nullptr);
	}

	VkSampler Texture::getSampler() const
	{
		return m_texture_sampler;
	}
	VkImage Texture::getImage() const
	{
		return m_texture_image;
	}
	VkImageView Texture::getImageView() const
	{
		return m_texture_image_view;
	}
	VkDescriptorImageInfo Texture::getImageInfo() const
	{
		return m_descriptor;
	}
	VkImageLayout Texture::getImageLayout() const
	{
		return m_texture_layout;
	}
	VkExtent3D Texture::getExtent() const
	{
		return m_extent;
	}
	VkFormat Texture::getFormat() const
	{
		return m_format;
	}

	void Texture::updateDescriptor()
	{
		m_descriptor.sampler = m_texture_sampler;
		m_descriptor.imageView = m_texture_image_view;
		m_descriptor.imageLayout = m_texture_layout;
	}

	void Texture::createTextureImage(const void* source, const unsigned int tex_height, const unsigned int tex_width, const VkFormat format)
	{
		m_image_type = tex_height == 1 ? VK_IMAGE_TYPE_1D : VK_IMAGE_TYPE_2D;
		m_bytes_per_pixel = formatToBytesPerPixel(format);
		VkDeviceSize image_size = tex_height * tex_width * m_bytes_per_pixel;

		m_mip_levels = 1;

		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;

		m_device->createBuffer(
			image_size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&staging_buffer,
			&staging_buffer_memory
		);

		void* data;
		vkMapMemory(m_device->getDevice(), staging_buffer_memory, 0, image_size, 0, &data);
		memcpy(data, source, image_size);
		vkUnmapMemory(m_device->getDevice(), staging_buffer_memory);

		m_format = format;
		m_extent = { tex_width, tex_height, 1 };

		VkImageCreateInfo image_info{};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = m_image_type;
		image_info.extent = m_extent;
		image_info.mipLevels = m_mip_levels;
		image_info.arrayLayers = m_layer_count;
		image_info.format = format;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.flags = 0;

		m_device->createImageWithInfo(
			&image_info,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&m_texture_image,
			&m_texture_image_memory
		);
		m_device->transitionImageLayout(
			m_texture_image,
			format,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			m_mip_levels,
			m_layer_count
		);
		m_device->copyBufferToImage(
			staging_buffer,
			m_texture_image,
			tex_width,
			tex_height,
			m_layer_count
		);

		m_device->transitionImageLayout(
			m_texture_image,
			format,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			m_mip_levels,
			m_layer_count
		);

		m_texture_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		vkDestroyBuffer(m_device->getDevice(), staging_buffer, nullptr);
		vkFreeMemory(m_device->getDevice(), staging_buffer_memory, nullptr);
	}

	void Texture::createTextureImageView()
	{
		VkImageViewCreateInfo view_info{};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = m_texture_image;
		view_info.viewType = static_cast<VkImageViewType>(m_image_type);
		view_info.format = m_format;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = m_mip_levels;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = m_layer_count;

		if (vkCreateImageView(m_device->getDevice(), &view_info, nullptr, &m_texture_image_view) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture image view!");
		}
	}

	void Texture::createTextureSampler(VkFilter filter, VkSamplerAddressMode address_mode)
	{
		VkSamplerCreateInfo sampler_info{};
		sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_info.magFilter = filter;
		sampler_info.minFilter = filter;

		sampler_info.addressModeU = address_mode;
		sampler_info.addressModeV = address_mode;
		sampler_info.addressModeW = address_mode;

		sampler_info.anisotropyEnable = VK_FALSE;
		sampler_info.maxAnisotropy = 1.0f;
		sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		sampler_info.unnormalizedCoordinates = VK_FALSE;

		// these fields useful for percentage close filtering for shadow maps
		sampler_info.compareEnable = VK_FALSE;
		sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;

		sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler_info.mipLodBias = 0.0f;
		sampler_info.minLod = 0.0f;
		sampler_info.maxLod = static_cast<float>(m_mip_levels);

		if (vkCreateSampler(m_device->getDevice(), &sampler_info, nullptr, &m_texture_sampler) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture sampler!");
		}
	}

	constexpr const unsigned int Texture::formatToBytesPerPixel(const VkFormat image_format)
	{
		switch (image_format)
		{
		case VK_FORMAT_R8_UNORM:
		case VK_FORMAT_R8_SNORM:
		case VK_FORMAT_R8_SRGB:
			return 1;
		case VK_FORMAT_R8G8_SINT:
		case VK_FORMAT_R8G8_SRGB:
		case VK_FORMAT_R8G8_SNORM:
		case VK_FORMAT_R8G8_UNORM:
			return 2;
		case VK_FORMAT_R8G8B8_SRGB:
			return 3;
		case VK_FORMAT_R8G8B8A8_SRGB:
			return 4;
		default:
			throw std::invalid_argument("Unsupported image format!");
		}
	}
}
