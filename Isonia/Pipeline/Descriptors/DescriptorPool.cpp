// internal
#include "Descriptors.h"

namespace Isonia::Pipeline::Descriptors
{
	DescriptorPool::Builder::Builder(Device* device, const unsigned int count)
		: m_device(device), m_pool_sizes(new VkDescriptorPoolSize[count]), m_pool_sizes_count(count)
	{
	}

	DescriptorPool::Builder* DescriptorPool::Builder::addPoolSize(const VkDescriptorType descriptor_type, const unsigned int count)
	{
		m_pool_sizes[m_pool_sizes_index++] = VkDescriptorPoolSize{ descriptor_type, count };
		return this;
	}

	DescriptorPool::Builder* DescriptorPool::Builder::setPoolFlags(const VkDescriptorPoolCreateFlags flags)
	{
		m_pool_flags = flags;
		return this;
	}

	DescriptorPool::Builder* DescriptorPool::Builder::setMaxSets(const unsigned int count)
	{
		m_max_sets = count;
		return this;
	}

	DescriptorPool* DescriptorPool::Builder::build() const
	{
		return new DescriptorPool(m_device, m_max_sets, m_pool_flags, m_pool_sizes, m_pool_sizes_count);
	}

	DescriptorPool::DescriptorPool(Device* device, unsigned int max_sets, VkDescriptorPoolCreateFlags pool_flags, const VkDescriptorPoolSize* pool_sizes, const unsigned int pool_sizes_count)
		: m_device(device)
	{
		VkDescriptorPoolCreateInfo descriptor_pool_info{};
		descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptor_pool_info.poolSizeCount = pool_sizes_count;
		descriptor_pool_info.pPoolSizes = pool_sizes;
		descriptor_pool_info.maxSets = max_sets;
		descriptor_pool_info.flags = pool_flags;

		if (vkCreateDescriptorPool(device->getDevice(), &descriptor_pool_info, nullptr, &m_descriptor_pool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor pool!");
		}
	}

	DescriptorPool::~DescriptorPool()
	{
		vkDestroyDescriptorPool(m_device->getDevice(), m_descriptor_pool, nullptr);
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
