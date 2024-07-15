// internal
#include "Descriptors.h"

// external
#include <stdexcept>

namespace Isonia::Pipeline::Descriptors
{
	DescriptorSetLayout::DescriptorSetLayout(Device* device, const unsigned int count)
		: m_device(device), m_bindings((VkDescriptorSetLayoutBinding*)malloc(count * sizeof(VkDescriptorSetLayoutBinding))), m_bindings_count(count)
	{
	};
	DescriptorSetLayout::~DescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(m_device->getDevice(), m_descriptor_set_layout, nullptr);
		free(m_bindings);
	}

	DescriptorSetLayout* DescriptorSetLayout::addBinding(unsigned int binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags, unsigned int count)
	{
		VkDescriptorSetLayoutBinding layout_binding{};
		layout_binding.binding = binding;
		layout_binding.descriptorType = descriptor_type;
		layout_binding.descriptorCount = count;
		layout_binding.stageFlags = stage_flags;
		m_bindings[binding] = layout_binding;
		return this;
	}

	DescriptorSetLayout* DescriptorSetLayout::build()
	{
		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
		descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_set_layout_info.bindingCount = m_bindings_count;
		descriptor_set_layout_info.pBindings = m_bindings;

		if (vkCreateDescriptorSetLayout(m_device->getDevice(), &descriptor_set_layout_info, nullptr, &m_descriptor_set_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor set layout!");
		}

		return this;
	}

	VkDescriptorSetLayout DescriptorSetLayout::getDescriptorSetLayout() const
	{
		return m_descriptor_set_layout;
	}
}
