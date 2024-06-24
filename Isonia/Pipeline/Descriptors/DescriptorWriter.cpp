// internal
#include "Descriptors.h"

namespace Isonia::Pipeline::Descriptors
{
	DescriptorWriter::DescriptorWriter(DescriptorSetLayout* set_layout, DescriptorPool* pool, const unsigned int count)
		: m_set_layout(set_layout), m_pool(pool), m_writes(new VkWriteDescriptorSet[count]), m_writes_count(count)
	{
	}

	DescriptorWriter* DescriptorWriter::writeBuffer(unsigned int binding, VkDescriptorBufferInfo* buffer_info)
	{
		const VkDescriptorSetLayoutBinding& bindingDescription = m_set_layout->m_bindings[binding];

		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = buffer_info;
		write.descriptorCount = 1;

		m_writes[binding] = write;
		return this;
	}

	DescriptorWriter* DescriptorWriter::writeImage(unsigned int binding, VkDescriptorImageInfo* image_info)
	{
		const VkDescriptorSetLayoutBinding& bindingDescription = m_set_layout->m_bindings[binding];

		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = image_info;
		write.descriptorCount = 1;

		m_writes[binding] = write;
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
		for (unsigned int i = 0; i < m_writes_count; i++)
		{
			m_writes[i].dstSet = *set;
		}
		vkUpdateDescriptorSets(m_pool->m_device->getDevice(), m_writes_count, m_writes, 0u, nullptr);
	}
}
