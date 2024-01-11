#pragma once

// internal
#include "DescriptorSetLayout.h"
#include "DescriptorPool.h"

// std
#include <vector>
#include <cassert>

namespace Isonia::Pipeline::Descriptors
{
	class DescriptorWriter
	{
	public:
		DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool)
			: setLayout(setLayout), pool(pool)
		{
		}

		DescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
		{
			assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

			auto& bindingDescription = setLayout.bindings[binding];

			assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.descriptorType = bindingDescription.descriptorType;
			write.dstBinding = binding;
			write.pBufferInfo = bufferInfo;
			write.descriptorCount = 1;

			writes.push_back(write);
			return *this;
		}

		DescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo)
		{
			assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

			auto& bindingDescription = setLayout.bindings[binding];

			assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.descriptorType = bindingDescription.descriptorType;
			write.dstBinding = binding;
			write.pImageInfo = imageInfo;
			write.descriptorCount = 1;

			writes.push_back(write);
			return *this;
		}

		bool Build(VkDescriptorSet& set)
		{
			bool success = pool.AllocateDescriptor(setLayout.GetDescriptorSetLayout(), set);
			if (!success)
			{
				return false;
			}
			Overwrite(set);
			return true;
		}

		void Overwrite(VkDescriptorSet& set)
		{
			for (auto& write : writes)
			{
				write.dstSet = set;
			}
			vkUpdateDescriptorSets(pool.device.GetDevice(), writes.size(), writes.data(), 0, nullptr);
		}

	private:
		DescriptorSetLayout& setLayout;
		DescriptorPool& pool;
		std::vector<VkWriteDescriptorSet> writes;
	};
}
