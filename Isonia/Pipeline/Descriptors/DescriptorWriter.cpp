// internal
#include "Descriptors.h"

// external
#include <stdexcept>

namespace Isonia::Pipeline::Descriptors
{
	DescriptorWriter::DescriptorWriter(DescriptorSetLayout* set_layout, DescriptorPool* pool, const unsigned int count)
		: m_set_layout(set_layout), m_pool(pool), m_writes(new VkWriteDescriptorSet[count]), m_writes_count(count)
	{
	}
	DescriptorWriter::~DescriptorWriter()
	{
		delete m_set_layout;
		delete m_pool;
		delete m_writes;
	}

	DescriptorWriter* DescriptorWriter::writeBuffer(unsigned int binding, VkDescriptorBufferInfo* buffer_info)
	{
		const VkDescriptorSetLayoutBinding binding_description = m_set_layout->m_bindings[binding];

		assert(binding_description.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = binding_description.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = buffer_info;
		write.descriptorCount = 1;

		m_writes[binding] = write;
		return this;
	}

	DescriptorWriter* DescriptorWriter::writeImage(unsigned int binding, VkDescriptorImageInfo* image_info)
	{
		const VkDescriptorSetLayoutBinding binding_description = m_set_layout->m_bindings[binding];

		assert(binding_description.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = binding_description.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = image_info;
		write.descriptorCount = 1;

		m_writes[binding] = write;
		return this;
	}

	DescriptorWriter* DescriptorWriter::build(VkDescriptorSet* set)
	{
		if (!m_pool->allocateDescriptor(m_set_layout->getDescriptorSetLayout(), set))
		{
			throw std::runtime_error("Failed to allocate descriptor!");
		}
		overwrite(set);
		return this;
	}

	void DescriptorWriter::overwrite(VkDescriptorSet* set)
	{
		for (unsigned int i = 0; i < m_writes_count; i++)
		{
			m_writes[i].dstSet = *set;
		}
		vkUpdateDescriptorSets(m_pool->m_device->getDevice(), m_writes_count, m_writes, 0u, nullptr);
	}

	DescriptorSetLayout* DescriptorWriter::getSetLayout() const
	{
		return m_set_layout;
	}
	DescriptorPool* DescriptorWriter::getPool() const
	{
		return m_pool;
	}
}
