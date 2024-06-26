// internal
#include "Renderable.h"

// external
#include <cstring>

namespace Isonia::Renderable
{
	Texture::Texture(Pipeline::Device* device, const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const unsigned int tex_width, const unsigned int tex_height)
		: m_device{ device }
	{
		createTextureImage(warp_noise, noise, tex_width, tex_height);
		createTextureImageView();
		createTextureSampler(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);
		updateDescriptor();
	}

	Texture::Texture(Pipeline::Device* device, const void* texture, const unsigned int tex_width, const unsigned int tex_height, const VkFormat format)
		: m_device{ device }
	{
		createTextureImage(texture, tex_width, tex_height, format);
		createTextureImageView();
		createTextureSampler(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
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

	Texture* Texture::createTextureFromNoise(Pipeline::Device* device, const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const unsigned int tex_width, const unsigned int tex_height)
	{
		return new Texture(device, warp_noise, noise, tex_width, tex_height);
	}

	Texture* Texture::createTextureFromPalette(Pipeline::Device* device, const Color* colors, const unsigned int tex_width)
	{
		return new Texture(device, (void*)colors, tex_width, 1, VK_FORMAT_R8G8B8A8_SRGB);
	}

	Texture* Texture::createTexture(Pipeline::Device* device, const void* texture, const unsigned int tex_width, const unsigned int tex_height)
	{
		return new Texture(device, texture, tex_width, tex_height, VK_FORMAT_R8G8B8A8_SRGB);
	}

	void Texture::updateDescriptor()
	{
		m_descriptor.sampler = m_texture_sampler;
		m_descriptor.imageView = m_texture_image_view;
		m_descriptor.imageLayout = m_texture_layout;
	}

	void Texture::createTextureImage(const Noise::VirtualWarpNoise* warp_noise, const Noise::VirtualNoise* noise, const unsigned int texWidth, const unsigned int texHeight)
	{
		unsigned char* pixels = new unsigned char[texWidth * texHeight];
		for (unsigned int h = 0; h < texHeight; h++)
		{
			const unsigned int h_i = h * texHeight;
			for (unsigned int w = 0; w < texWidth; w++)
			{
				const float s = h / static_cast<float>(texHeight);
				const float t = w / static_cast<float>(texWidth);

				float nx = Math::cosf(s * Math::two_pi) / (Math::two_pi);
				float ny = Math::cosf(t * Math::two_pi) / (Math::two_pi);
				float nz = Math::sinf(s * Math::two_pi) / (Math::two_pi);
				float nt = Math::sinf(t * Math::two_pi) / (Math::two_pi);

				const unsigned int i = h_i + w;
				warp_noise->transformCoordinate(&nx, &ny, &nz, &nt);
				const float noiseValue = noise->generateNoise(nx, ny, nz, nt);
				const float pushedValue = (noiseValue + 1.0f) * 0.5f;
				pixels[i] = static_cast<unsigned char>(pushedValue * 255.0f + 0.5f);
			}
		}
		createTextureImage(pixels, texWidth, texHeight, VK_FORMAT_R8_UNORM);
		delete[] pixels;
	}

	void Texture::createTextureImage(const void* source, const unsigned int tex_width, const unsigned int tex_height, const VkFormat format)
	{
		m_image_type = tex_height == 1 ? VK_IMAGE_TYPE_1D : VK_IMAGE_TYPE_2D;
		m_bytes_per_pixel = formatToBytesPerPixel(format);
		VkDeviceSize imageSize = tex_width * tex_height * m_bytes_per_pixel;

		m_mip_levels = 1;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		m_device->createBuffer(
			imageSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer,
			&stagingBufferMemory
		);

		void* data;
		vkMapMemory(m_device->getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, source, static_cast<size_t>(imageSize));
		vkUnmapMemory(m_device->getDevice(), stagingBufferMemory);

		m_format = format;
		m_extent = { tex_width, tex_height, 1 };

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = m_image_type;
		imageInfo.extent = m_extent;
		imageInfo.mipLevels = m_mip_levels;
		imageInfo.arrayLayers = m_layer_count;
		imageInfo.format = format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.flags = 0;

		m_device->createImageWithInfo(
			&imageInfo,
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
			stagingBuffer,
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

		vkDestroyBuffer(m_device->getDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_device->getDevice(), stagingBufferMemory, nullptr);
	}

	void Texture::createTextureImageView()
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_texture_image;
		viewInfo.viewType = static_cast<VkImageViewType>(m_image_type);
		viewInfo.format = m_format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = m_mip_levels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = m_layer_count;

		if (vkCreateImageView(m_device->getDevice(), &viewInfo, nullptr, &m_texture_image_view) != VK_SUCCESS)
		{
			//throw std::runtime_error("Failed to create texture image view!");
		}
	}

	void Texture::createTextureSampler(VkFilter filter, VkSamplerAddressMode addressMode)
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = filter;
		samplerInfo.minFilter = filter;

		samplerInfo.addressModeU = addressMode;
		samplerInfo.addressModeV = addressMode;
		samplerInfo.addressModeW = addressMode;

		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		// these fields useful for percentage close filtering for shadow maps
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(m_mip_levels);

		if (vkCreateSampler(m_device->getDevice(), &samplerInfo, nullptr, &m_texture_sampler) != VK_SUCCESS)
		{
			//throw std::runtime_error("Failed to create texture sampler!");
		}
	}

	constexpr const unsigned int Texture::formatToBytesPerPixel(const VkFormat imageFormat)
	{
		switch (imageFormat)
		{
		case VK_FORMAT_R8_UNORM:
		case VK_FORMAT_R8_SNORM:
		case VK_FORMAT_R8_SRGB:
			return 1;
		case VK_FORMAT_R8G8_SRGB:
			return 2;
		case VK_FORMAT_R8G8B8_SRGB:
			return 3;
		case VK_FORMAT_R8G8B8A8_SRGB:
			return 4;
		default:
			//throw std::invalid_argument("Unsupported image format!");
			break;
		}
	}
}
