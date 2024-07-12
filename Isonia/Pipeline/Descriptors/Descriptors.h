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
		DescriptorSetLayout(Device* device, const unsigned int count);
		~DescriptorSetLayout();

		DescriptorSetLayout() = delete;
		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

		DescriptorSetLayout* addBinding(unsigned int binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags, unsigned int count = 1u);
		DescriptorSetLayout* build();

		VkDescriptorSetLayout getDescriptorSetLayout() const;

		VkDescriptorSetLayoutBinding* m_bindings;
		const unsigned int m_bindings_count;

	private:
		Device* m_device;
		VkDescriptorSetLayout m_descriptor_set_layout;

		friend struct DescriptorWriter;
	};

	struct DescriptorPool
	{
	public:
		DescriptorPool(Device* device, const unsigned int count);
		~DescriptorPool();

		DescriptorPool() = delete;
		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;

		DescriptorPool* addPoolSize(const VkDescriptorType descriptor_type, const unsigned int count);
		DescriptorPool* build(const unsigned int max_sets = 1024, const VkDescriptorPoolCreateFlags pool_flags = 0);

		bool allocateDescriptor(const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet* descriptor) const;
		void freeDescriptors(const VkDescriptorSet* descriptors, const unsigned int descriptors_count) const;
		void resetPool();

	private:
		Device* m_device;
		VkDescriptorPoolSize* m_pool_sizes;
		const unsigned int m_pool_sizes_count;
		unsigned int m_pool_sizes_index = 0;

		VkDescriptorPool m_descriptor_pool;

		friend struct DescriptorWriter;
	};

	struct DescriptorWriter
	{
	public:
		DescriptorWriter(DescriptorSetLayout* set_layout, DescriptorPool* pool, const unsigned int count);
		~DescriptorWriter();

		DescriptorWriter() = delete;
		DescriptorWriter(const DescriptorWriter&) = delete;
		DescriptorWriter& operator=(const DescriptorWriter&) = delete;

		DescriptorWriter* writeBuffer(unsigned int binding, VkDescriptorBufferInfo* buffer_info);
		DescriptorWriter* writeImage(unsigned int binding, VkDescriptorImageInfo* image_info);

		DescriptorWriter* build(VkDescriptorSet* set);
		void overwrite(VkDescriptorSet* set);

		DescriptorSetLayout* getSetLayout() const;
		DescriptorPool* getPool() const;

	private:
		DescriptorSetLayout* m_set_layout;
		DescriptorPool* m_pool;
		VkWriteDescriptorSet* m_writes;
		const unsigned int m_writes_count;
	};
}
