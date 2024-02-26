#pragma once

// internal
#include "../Pipeline/Device.h"
#include "../../Renderable/Color/Color.h"
#include "../Noise/Noise.h"
#include "../Noise/WarpNoise.h"

// external
#define STB_IMAGE_IMPLEMENTATION
#include "../../External/stb/stb_image.h"
#include <vulkan/vulkan.h>

// std
#include <cmath>
#include <stdexcept>
#include <memory>
#include <string>

namespace Isonia::Renderable
{
	class Texture
	{
	public:
		Texture(Pipeline::Device& device, const std::string& textureFilepath) : device{ device }
		{
			CreateTextureImage(textureFilepath);
			CreateTextureImageView();
			CreateTextureSampler(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);
			UpdateDescriptor();
		}

		Texture(Pipeline::Device& device, const Noise::WarpNoise& warpNoise, const Noise::Noise& noise, const uint32_t texWidth, const uint32_t texHeight) : device{ device }
		{
			CreateTextureImage(warpNoise, noise, texWidth, texHeight);
			CreateTextureImageView();
			CreateTextureSampler(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);
			UpdateDescriptor();
		}

		Texture(Pipeline::Device& device, const void* texture, const uint32_t texWidth, const uint32_t texHeight, const VkFormat format) : device{ device }
		{
			CreateTextureImage(texture, texWidth, texHeight, format);
			CreateTextureImageView();
			CreateTextureSampler(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
			UpdateDescriptor();
		}

		~Texture()
		{
			vkDestroySampler(device.GetDevice(), textureSampler, nullptr);
			vkDestroyImageView(device.GetDevice(), textureImageView, nullptr);
			vkDestroyImage(device.GetDevice(), textureImage, nullptr);
			vkFreeMemory(device.GetDevice(), textureImageMemory, nullptr);
		}

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		VkSampler GetSampler() const { return textureSampler; }
		VkImage GetImage() const { return textureImage; }
		VkImageView GetImageView() const { return textureImageView; }
		VkDescriptorImageInfo GetImageInfo() const { return descriptor; }
		VkImageLayout GetImageLayout() const { return textureLayout; }
		VkExtent3D GetExtent() const { return extent; }
		VkFormat GetFormat() const { return format; }

		static Texture* CreateTextureFromFile(Pipeline::Device& device, const std::string& filepath)
		{
			return new Texture(device, filepath);
		}

		static Texture* CreateTextureFromNoise(Pipeline::Device& device, const Noise::WarpNoise& warpNoise, const Noise::Noise& noise, const uint32_t texWidth, const uint32_t texHeight)
		{
			return new Texture(device, warpNoise, noise, texWidth, texHeight);
		}

		static Texture* CreateTextureFromPalette(Pipeline::Device& device, const Renderable::Color::Color colors[], const uint32_t texWidth)
		{
			return new Texture(device, (void*)colors, texWidth, 1, VK_FORMAT_R8G8B8A8_SRGB);
		}

		static Texture* CreateTexture(Pipeline::Device& device, const void* texture, const uint32_t texWidth, const uint32_t texHeight)
		{
			return new Texture(device, texture, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB);
		}

		void UpdateDescriptor()
		{
			descriptor.sampler = textureSampler;
			descriptor.imageView = textureImageView;
			descriptor.imageLayout = textureLayout;
		}

	private:
		void CreateTextureImage(const std::string& filepath)
		{
			int texWidth, texHeight, texChannels;
			// stbi_set_flip_vertically_on_load(1);  // todo determine why texture coordinates are flipped
			stbi_uc* pixels = stbi_load(filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

			if (!pixels)
			{
				throw std::runtime_error("Failed to load texture image!");
			}

			CreateTextureImage(pixels, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB);

			stbi_image_free(pixels);
		}

		void CreateTextureImage(const Noise::WarpNoise& warpNoise, const Noise::Noise& noise, const uint32_t texWidth, const uint32_t texHeight)
		{
			uint8_t* pixels = new uint8_t[texWidth * texHeight];
			for (uint32_t h = 0; h < texHeight; h++)
			{
				const uint32_t h_i = h * texHeight;
				for (uint32_t w = 0; w < texWidth; w++)
				{
					const float s = h / static_cast<float>(texHeight);
					const float t = w / static_cast<float>(texWidth);

					float nx = cos(s * 2.0f * IMath::PI) / (2.0f * IMath::PI);
					float ny = cos(t * 2.0f * IMath::PI) / (2.0f * IMath::PI);
					float nz = sin(s * 2.0f * IMath::PI) / (2.0f * IMath::PI);
					float nt = sin(t * 2.0f * IMath::PI) / (2.0f * IMath::PI);

					const uint32_t i = h_i + w;
					warpNoise.TransformCoordinate(nx, ny, nz, nt);
					const float noiseValue = noise.GenerateNoise(nx, ny, nz, nt);
					const float pushedValue = (noiseValue + 1.0f) * 0.5f;
					pixels[i] = static_cast<uint8_t>(pushedValue * 255.0f + 0.5f);
				}
			}
			CreateTextureImage(pixels, texWidth, texHeight, VK_FORMAT_R8_UNORM);
			delete pixels;
		}

		void CreateTextureImage(const void* source, const uint32_t texWidth, const uint32_t texHeight, const VkFormat format)
		{
			imageType = texHeight == 1 ? VK_IMAGE_TYPE_1D : VK_IMAGE_TYPE_2D;
			bytesPerPixel = FormatToBytesPerPixel(format);
			VkDeviceSize imageSize = texWidth * texHeight * bytesPerPixel;

			// mMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
			mipLevels = 1;

			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;

			device.CreateBuffer(
				imageSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				stagingBuffer,
				stagingBufferMemory
			);

			void* data;
			vkMapMemory(device.GetDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
			memcpy(data, source, static_cast<size_t>(imageSize));
			vkUnmapMemory(device.GetDevice(), stagingBufferMemory);

			this->format = format;
			extent = { texWidth, texHeight, 1 };

			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = imageType;
			imageInfo.extent = extent;
			imageInfo.mipLevels = mipLevels;
			imageInfo.arrayLayers = layerCount;
			imageInfo.format = format;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.flags = 0;

			device.CreateImageWithInfo(
				imageInfo,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				textureImage,
				textureImageMemory
			);
			device.TransitionImageLayout(
				textureImage,
				format,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				mipLevels,
				layerCount
			);
			device.CopyBufferToImage(
				stagingBuffer,
				textureImage,
				texWidth,
				texHeight,
				layerCount
			);

			// comment this out if using mips
			device.TransitionImageLayout(
				textureImage,
				format,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				mipLevels,
				layerCount
			);

			// If we generate mip maps then the final image will alerady be READ_ONLY_OPTIMAL
			// device.GenerateMipmaps(textureImage, format, texWidth, texHeight, mipLevels);
			textureLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			vkDestroyBuffer(device.GetDevice(), stagingBuffer, nullptr);
			vkFreeMemory(device.GetDevice(), stagingBufferMemory, nullptr);
		}

		void CreateTextureImageView()
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = textureImage;
			viewInfo.viewType = static_cast<VkImageViewType>(imageType);
			viewInfo.format = format;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = mipLevels;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = layerCount;

			if (vkCreateImageView(device.GetDevice(), &viewInfo, nullptr, &textureImageView) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create texture image view!");
			}
		}

		void CreateTextureSampler(VkFilter filter, VkSamplerAddressMode addressMode)
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
			samplerInfo.maxLod = static_cast<float>(mipLevels);

			if (vkCreateSampler(device.GetDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create texture sampler!");
			}
		}

		static constexpr const uint32_t FormatToBytesPerPixel(const VkFormat imageFormat)
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
				throw std::invalid_argument("Unsupported image format!");
			}
		}

		VkDescriptorImageInfo descriptor{};
		Pipeline::Device& device;
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;
		VkFormat format;
		VkImageLayout textureLayout;
		VkImageType imageType;
		uint32_t bytesPerPixel;
		uint32_t mipLevels{ 1 };
		uint32_t layerCount{ 1 };
		VkExtent3D extent{};
	};
}
