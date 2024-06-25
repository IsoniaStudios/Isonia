#pragma once

// internal
#include "../Pipeline.h"

// external
#include <vulkan/vulkan.h>

namespace Isonia::Pipeline::Descriptors
{
	struct DescriptorSetLayout
	{
	public:
		struct Builder
		{
		public:
			Builder(Device* device, const unsigned int count);

			Builder* addBinding(unsigned int binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags, unsigned int count = 1);
			DescriptorSetLayout* build() const;

		private:
			Device* m_device;
			VkDescriptorSetLayoutBinding* m_bindings;
			unsigned int m_bindings_count;
		};

		DescriptorSetLayout(Device* device, const VkDescriptorSetLayoutBinding* bindings, const unsigned int bindings_count);
		~DescriptorSetLayout();

		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

		VkDescriptorSetLayout getDescriptorSetLayout() const;

		const VkDescriptorSetLayoutBinding* m_bindings;
		const unsigned int m_bindings_count;

	private:
		Device* m_device;
		VkDescriptorSetLayout m_descriptor_set_layout;

		friend struct DescriptorWriter;
	};

	struct DescriptorPool
	{
	public:
		struct Builder
		{
		public:
			Builder(Device* device, const unsigned int count);

			Builder* addPoolSize(const VkDescriptorType descriptor_type, const unsigned int count);
			Builder* setPoolFlags(const VkDescriptorPoolCreateFlags flags);
			Builder* setMaxSets(const unsigned int count);
			DescriptorPool* build() const;

		private:
			Device* m_device;
			VkDescriptorPoolSize* m_pool_sizes;
			unsigned int m_pool_sizes_count;
			unsigned int m_pool_sizes_index = 0;
			unsigned int m_max_sets = 1024;
			VkDescriptorPoolCreateFlags m_pool_flags = 0;
		};

		DescriptorPool(Device* device, unsigned int max_sets, VkDescriptorPoolCreateFlags pool_flags, const VkDescriptorPoolSize* pool_sizes, const unsigned int pool_sizes_count);
		~DescriptorPool();

		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;

		bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet* descriptor) const;
		void freeDescriptors(const VkDescriptorSet* descriptors, const unsigned int descriptors_count) const;
		void resetPool();

	private:
		Device* m_device;
		VkDescriptorPool m_descriptor_pool;

		friend struct DescriptorWriter;
	};

	struct DescriptorWriter
	{
	public:
		DescriptorWriter(DescriptorSetLayout* set_layout, DescriptorPool* pool, const unsigned int count);

		DescriptorWriter* writeBuffer(unsigned int binding, VkDescriptorBufferInfo* buffer_info);
		DescriptorWriter* writeImage(unsigned int binding, VkDescriptorImageInfo* image_info);

		bool build(VkDescriptorSet* set);
		void overwrite(VkDescriptorSet* set);

	private:
		DescriptorSetLayout* m_set_layout;
		DescriptorPool* m_pool;
		VkWriteDescriptorSet* m_writes;
		unsigned int m_writes_count;
	};
}
