// internal
#include "Descriptors.h"

namespace Isonia::Pipeline::Descriptors
{
	DescriptorWriter::DescriptorWriter(DescriptorSetLayout* set_layout, DescriptorPool* pool)
		: m_set_layout(set_layout), m_pool(pool)
	{
	}

	DescriptorWriter* DescriptorWriter::writeBuffer(unsigned int binding, VkDescriptorBufferInfo* buffer_info)
	{
		assert(m_set_layout->m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

		VkDescriptorSetLayoutBinding& bindingDescription = m_set_layout->m_bindings[binding];

		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = buffer_info;
		write.descriptorCount = 1;

		m_writes.push_back(write);
		return this;
	}

	DescriptorWriter* DescriptorWriter::writeImage(unsigned int binding, VkDescriptorImageInfo* image_info)
	{
		assert(m_set_layout->m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

		VkDescriptorSetLayoutBinding& bindingDescription = m_set_layout->m_bindings[binding];

		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = image_info;
		write.descriptorCount = 1;

		m_writes.push_back(write);
		return this;
	}

	bool DescriptorWriter::build(VkDescriptorSet* set)
	{
		bool success = m_pool->allocateDescriptor(m_set_layout->getDescriptorSetLayout(), set);
		if (!success)
		{
			return false;
		}
		overwrite(set);
		return true;
	}

	void DescriptorWriter::overwrite(VkDescriptorSet* set)
	{
		for (auto& write : m_writes)
		{
			write.dstSet = *set;
		}
		vkUpdateDescriptorSets(m_pool->m_device->getDevice(), static_cast<unsigned int>(m_writes.size()), m_writes.data(), 0, nullptr);
	}
}
