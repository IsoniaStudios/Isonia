#pragma once

// internal
#include "../Device.h"

// std
#include <memory>
#include <vector>

namespace Isonia::Pipeline::Descriptors
{
	class DescriptorPool
	{
	public:
		class Builder
		{
		public:
			Builder(Device& device) : device(device)
			{
			}

			Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count)
			{
				poolSizes.push_back({ descriptorType, count });
				return *this;
			}

			Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags)
			{
				poolFlags = flags;
				return *this;
			}

			Builder& SetMaxSets(uint32_t count)
			{
				maxSets = count;
				return *this;
			}

			std::unique_ptr<DescriptorPool> Build() const
			{
				return std::make_unique<DescriptorPool>(device, maxSets, poolFlags, poolSizes);
			}

		private:
			Device& device;
			std::vector<VkDescriptorPoolSize> poolSizes{};
			uint32_t maxSets = 1000;
			VkDescriptorPoolCreateFlags poolFlags = 0;
		};

		DescriptorPool(Device& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes)
			: device(device)
		{
			VkDescriptorPoolCreateInfo descriptorPoolInfo{};
			descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			descriptorPoolInfo.pPoolSizes = poolSizes.data();
			descriptorPoolInfo.maxSets = maxSets;
			descriptorPoolInfo.flags = poolFlags;

			if (vkCreateDescriptorPool(device.GetDevice(), &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create descriptor pool!");
			}
		}

		~DescriptorPool()
		{
			vkDestroyDescriptorPool(device.GetDevice(), descriptorPool, nullptr);
		}

		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;

		bool AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
		{
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = descriptorPool;
			allocInfo.pSetLayouts = &descriptorSetLayout;
			allocInfo.descriptorSetCount = 1;

			// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
			// a new pool whenever an old pool fills up. But this is beyond our current scope
			if (vkAllocateDescriptorSets(device.GetDevice(), &allocInfo, &descriptor) != VK_SUCCESS) {
				return false;
			}
			return true;
		}

		void FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
		{
			vkFreeDescriptorSets(
				device.GetDevice(),
				descriptorPool,
				static_cast<uint32_t>(descriptors.size()),
				descriptors.data()
			);
		}

		void ResetPool()
		{
			vkResetDescriptorPool(device.GetDevice(), descriptorPool, 0);
		}

	private:
		Device& device;
		VkDescriptorPool descriptorPool;

		friend class DescriptorWriter;
	};
}
