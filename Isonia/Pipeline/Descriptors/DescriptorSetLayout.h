#pragma once

// internal
#include "../Device.h"

// std
#include <memory>
#include <unordered_map>
#include <cassert>

namespace Isonia::Pipeline::Descriptors
{
	class DescriptorSetLayout
	{
	public:
		class Builder
		{
		public:
			Builder(Device& device) : device(device)
			{
			};

			Builder& AddBinding(
				uint32_t binding,
				VkDescriptorType descriptorType,
				VkShaderStageFlags stageFlags,
				uint32_t count = 1)
			{
				assert(bindings.count(binding) == 0 && "Binding already in use");
				VkDescriptorSetLayoutBinding layoutBinding{};
				layoutBinding.binding = binding;
				layoutBinding.descriptorType = descriptorType;
				layoutBinding.descriptorCount = count;
				layoutBinding.stageFlags = stageFlags;
				bindings[binding] = layoutBinding;
				return *this;
			}

			DescriptorSetLayout* Build() const
			{
				return new DescriptorSetLayout(device, bindings);
			}

		private:
			Device& device;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
		};

		DescriptorSetLayout(Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
			: device(device), bindings(bindings)
		{
			std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
			for (auto kv : bindings)
			{
				setLayoutBindings.push_back(kv.second);
			}

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
			descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
			descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

			if (vkCreateDescriptorSetLayout(
				device.GetDevice(),
				&descriptorSetLayoutInfo,
				nullptr,
				&descriptorSetLayout) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create descriptor set layout!");
			}
		}

		~DescriptorSetLayout()
		{
			vkDestroyDescriptorSetLayout(device.GetDevice(), descriptorSetLayout, nullptr);
		}

		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

		VkDescriptorSetLayout GetDescriptorSetLayout() const { return descriptorSetLayout; }

	private:
		Device& device;
		VkDescriptorSetLayout descriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

		friend class DescriptorWriter;
	};
}
