// internal
#include "Descriptors.h"

namespace Isonia::Pipeline::Descriptors
{
	DescriptorSetLayout::Builder::Builder(Device* device, const unsigned int count)
		: m_device(device), m_bindings(new VkDescriptorSetLayoutBinding[count]), m_bindings_count(count)
	{
	};

	DescriptorSetLayout::Builder* DescriptorSetLayout::Builder::addBinding(unsigned int binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags, unsigned int count)
	{
		VkDescriptorSetLayoutBinding layout_binding{};
		layout_binding.binding = binding;
		layout_binding.descriptorType = descriptor_type;
		layout_binding.descriptorCount = count;
		layout_binding.stageFlags = stage_flags;
		m_bindings[binding] = layout_binding;
		return this;
	}

	DescriptorSetLayout* DescriptorSetLayout::Builder::build() const
	{
		return new DescriptorSetLayout(m_device, m_bindings, m_bindings_count);
	}

	DescriptorSetLayout::DescriptorSetLayout(Device* device, const VkDescriptorSetLayoutBinding* bindings, const unsigned int bindings_count)
		: m_device(device), m_bindings(bindings), m_bindings_count(bindings_count)
	{
		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
		descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_set_layout_info.bindingCount = bindings_count;
		descriptor_set_layout_info.pBindings = bindings;

		if (vkCreateDescriptorSetLayout(
			m_device->getDevice(),
			&descriptor_set_layout_info,
			nullptr,
			&m_descriptor_set_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor set layout!");
		}
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(m_device->getDevice(), m_descriptor_set_layout, nullptr);
	}

	VkDescriptorSetLayout DescriptorSetLayout::getDescriptorSetLayout() const
	{
		return m_descriptor_set_layout;
	}
}
