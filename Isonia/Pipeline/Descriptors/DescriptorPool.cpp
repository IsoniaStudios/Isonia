// internal
#include "Descriptors.h"

// external
#include <stdexcept>

namespace Isonia::Pipeline::Descriptors
{
	DescriptorPool::DescriptorPool(Device* device, const unsigned int count)
		: m_device(device), m_pool_sizes((VkDescriptorPoolSize*) malloc(count * sizeof(VkDescriptorPoolSize))), m_pool_sizes_count(count)
	{
	}
	DescriptorPool::~DescriptorPool()
	{
		vkDestroyDescriptorPool(m_device->getDevice(), m_descriptor_pool, nullptr);
		free(m_pool_sizes);
	}

	DescriptorPool* DescriptorPool::addPoolSize(const VkDescriptorType descriptor_type, const unsigned int count)
	{
		m_pool_sizes[m_pool_sizes_index++] = VkDescriptorPoolSize{ descriptor_type, count };
		return this;
	}

	DescriptorPool* DescriptorPool::build(const unsigned int max_sets, const VkDescriptorPoolCreateFlags pool_flags)
	{
		VkDescriptorPoolCreateInfo descriptor_pool_info{};
		descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptor_pool_info.poolSizeCount = m_pool_sizes_count;
		descriptor_pool_info.pPoolSizes = m_pool_sizes;
		descriptor_pool_info.maxSets = max_sets;
		descriptor_pool_info.flags = pool_flags;

		if (vkCreateDescriptorPool(m_device->getDevice(), &descriptor_pool_info, nullptr, &m_descriptor_pool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor pool!");
		}

		return this;
	}

	bool DescriptorPool::allocateDescriptor(const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet* descriptor) const
	{
		VkDescriptorSetAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloc_info.descriptorPool = m_descriptor_pool;
		alloc_info.pSetLayouts = &descriptor_set_layout;
		alloc_info.descriptorSetCount = 1;

		if (vkAllocateDescriptorSets(m_device->getDevice(), &alloc_info, descriptor) != VK_SUCCESS)
		{
			return false;
		}
		return true;
	}

	void DescriptorPool::freeDescriptors(const VkDescriptorSet* descriptors, const unsigned int descriptors_count) const
	{
		vkFreeDescriptorSets(
			m_device->getDevice(),
			m_descriptor_pool,
			descriptors_count,
			descriptors
		);
	}

	void DescriptorPool::resetPool()
	{
		vkResetDescriptorPool(m_device->getDevice(), m_descriptor_pool, 0);
	}
}
