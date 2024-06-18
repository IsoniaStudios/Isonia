#pragma once

// internal
#include "../Pipeline.h"

// external
#include <vulkan/vulkan.h>

// std
#include <cassert>
#include <stdexcept>

#include <vector>
#include <unordered_map>

namespace Isonia::Pipeline::Descriptors
{
	class DescriptorSetLayout
	{
	public:
		class Builder
		{
		public:
			Builder(Device* device);

			Builder* addBinding(unsigned int binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags, unsigned int count = 1);
			DescriptorSetLayout* build() const;

		private:
			Device* m_device;
			std::unordered_map<unsigned int, VkDescriptorSetLayoutBinding> m_bindings{};
		};

		DescriptorSetLayout(Device* device, std::unordered_map<unsigned int, VkDescriptorSetLayoutBinding> bindings);
		~DescriptorSetLayout();

		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

		VkDescriptorSetLayout getDescriptorSetLayout() const;

	private:
		Device* m_device;
		VkDescriptorSetLayout m_descriptor_set_layout;
		std::unordered_map<unsigned int, VkDescriptorSetLayoutBinding> m_bindings;

		friend class DescriptorWriter;
	};

	class DescriptorPool
	{
	public:
		class Builder
		{
		public:
			Builder(Device* device);

			Builder* addPoolSize(VkDescriptorType descriptor_type, unsigned int count);
			Builder* setPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder* setMaxSets(unsigned int count);
			DescriptorPool* build() const;

		private:
			Device* m_device;
			std::vector<VkDescriptorPoolSize> m_pool_sizes{};
			unsigned int m_max_sets = 1000;
			VkDescriptorPoolCreateFlags m_pool_flags = 0;
		};

		DescriptorPool(Device* device, unsigned int max_sets, VkDescriptorPoolCreateFlags pool_flags, const std::vector<VkDescriptorPoolSize>* pool_sizes);
		~DescriptorPool();

		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;

		bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet* descriptor) const;
		void freeDescriptors(std::vector<VkDescriptorSet>* descriptors) const;
		void resetPool();

	private:
		Device* m_device;
		VkDescriptorPool m_descriptor_pool;

		friend class DescriptorWriter;
	};

	class DescriptorWriter
	{
	public:
		DescriptorWriter(DescriptorSetLayout* set_layout, DescriptorPool* pool);

		DescriptorWriter* writeBuffer(unsigned int binding, VkDescriptorBufferInfo* buffer_info);
		DescriptorWriter* writeImage(unsigned int binding, VkDescriptorImageInfo* image_info);

		bool build(VkDescriptorSet* set);
		void overwrite(VkDescriptorSet* set);

	private:
		DescriptorSetLayout* m_set_layout;
		DescriptorPool* m_pool;
		std::vector<VkWriteDescriptorSet> m_writes;
	};
}
