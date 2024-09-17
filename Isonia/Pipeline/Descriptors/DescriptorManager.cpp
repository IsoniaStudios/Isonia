// internal
#include "Descriptors.h"

namespace Isonia::Pipeline::Descriptors
{
	DescriptorManager::DescriptorManager(Device* device, const unsigned int size)
		: m_pool({ device, size }), m_set_layout({ device, size }),
		// There has to be a better way no??? wtf is this???
		// There is, a uniun and friend descriptowriter with private copy constructor
		// OR, go the c way with sizeof(descriptorwiter) * length of char[]
		m_writers{ createWriter(size), createWriter(size), createWriter(size) }
	{
	}
	DescriptorManager::~DescriptorManager()
	{
	}

	DescriptorWriter DescriptorManager::createWriter(const unsigned int index)
	{
		return { getSetLayout(), getPool(), index };
	}

	DescriptorPool* DescriptorManager::getPool()
	{
		return &m_pool;
	}
	DescriptorSetLayout* DescriptorManager::getSetLayout()
	{
		return &m_set_layout;
	}
	DescriptorWriter* DescriptorManager::getWriters(unsigned int index)
	{
		return &m_writers[index];
	}
	VkDescriptorSet* DescriptorManager::getDescriptorSets(unsigned int index)
	{
		return &m_descriptor_sets[index];
	}
}
