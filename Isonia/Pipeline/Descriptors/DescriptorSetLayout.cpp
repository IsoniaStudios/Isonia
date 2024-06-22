// internal
#include "Descriptors.h"

namespace Isonia::Pipeline::Descriptors
{
	DescriptorSetLayout::Builder::Builder(Device* device)
		: m_device(device)
	{
	};

	DescriptorSetLayout::Builder* DescriptorSetLayout::Builder::addBinding(unsigned int binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags, unsigned int count)
	{
		assert(m_bindings.count(binding) == 0 && "Binding already in use");
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
		return new DescriptorSetLayout(m_device, m_bindings);
	}

	DescriptorSetLayout::DescriptorSetLayout(Device* device, std::unordered_map<unsigned int, VkDescriptorSetLayoutBinding> bindings)
		: m_device(device), m_bindings(bindings)
	{
		VkDescriptorSetLayoutBinding* set_layout_bindings = new VkDescriptorSetLayoutBinding[bindings.size()];

		unsigned int index = 0;
		for (const auto& kv : bindings)
		{
			set_layout_bindings[index++] = kv.second;
		}

		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
		descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_set_layout_info.bindingCount = index;
		descriptor_set_layout_info.pBindings = set_layout_bindings;

		if (vkCreateDescriptorSetLayout(
			m_device->getDevice(),
			&descriptor_set_layout_info,
			nullptr,
			&m_descriptor_set_layout) != VK_SUCCESS)
		{
			delete[] set_layout_bindings;
			throw std::runtime_error("Failed to create descriptor set layout!");
		}
		delete[] set_layout_bindings;
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
