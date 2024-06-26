// internal
#include "Pipeline.h"

// external
#include <cstring>

namespace Isonia::Pipeline
{
    Buffer::Buffer(Device* device, VkDeviceSize instance_size, unsigned int instance_count, VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags memory_property_flags, VkDeviceSize min_offset_alignment)
        : m_device(device), m_instance_size(instance_size), m_instance_count(instance_count), m_usage_flags(usage_flags), m_memory_property_flags(memory_property_flags)
    {
        m_alignment_size = getAlignment(instance_size, min_offset_alignment);
        m_buffer_size = m_alignment_size * instance_count;
        device->createBuffer(m_buffer_size, usage_flags, memory_property_flags, &m_buffer, &m_memory);
    }

    Buffer::~Buffer()
    {
        unmap();
        vkDestroyBuffer(m_device->getDevice(), m_buffer, nullptr);
        vkFreeMemory(m_device->getDevice(), m_memory, nullptr);
    }

    VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset)
    {
        assert(m_buffer && m_memory && "Called map on buffer before create");
        return vkMapMemory(m_device->getDevice(), m_memory, offset, size, 0, &m_mapped);
    }

    void Buffer::unmap()
    {
        if (m_mapped)
        {
            vkUnmapMemory(m_device->getDevice(), m_memory);
            m_mapped = nullptr;
        }
    }

    void Buffer::writeToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
    {
        assert(m_mapped && "Cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE)
        {
            memcpy(m_mapped, data, m_buffer_size);
        }
        else
        {
            char* memOffset = (char*)m_mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset)
    {
        VkMappedMemoryRange mapped_range = {};
        mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mapped_range.memory = m_memory;
        mapped_range.offset = offset;
        mapped_range.size = size;
        return vkFlushMappedMemoryRanges(m_device->getDevice(), 1, &mapped_range);
    }

    VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
    {
        VkMappedMemoryRange mapped_range = {};
        mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mapped_range.memory = m_memory;
        mapped_range.offset = offset;
        mapped_range.size = size;
        return vkInvalidateMappedMemoryRanges(m_device->getDevice(), 1, &mapped_range);
    }

    VkDescriptorBufferInfo Buffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset)
    {
        return VkDescriptorBufferInfo{
            m_buffer,
            offset,
            size,
        };
    }

    void Buffer::writeToIndex(void* data, int index)
    {
        writeToBuffer(data, m_instance_size, index * m_alignment_size);
    }

    VkResult Buffer::flushIndex(int index)
    {
        return flush(m_alignment_size, index * m_alignment_size);
    }

    VkDescriptorBufferInfo Buffer::descriptorInfoForIndex(int index)
    {
        return descriptorInfo(m_alignment_size, index * m_alignment_size);
    }

    VkResult Buffer::invalidateIndex(int index)
    {
        return invalidate(m_alignment_size, index * m_alignment_size);
    }

    VkBuffer Buffer::getBuffer() const
    {
        return m_buffer;
    }
    void* Buffer::getMappedMemory() const
    {
        return m_mapped;
    }
    unsigned int Buffer::getInstanceCount() const
    {
        return m_instance_count;
    }
    VkDeviceSize Buffer::getInstanceSize() const
    {
        return m_instance_size;
    }
    VkDeviceSize Buffer::getAlignmentSize() const
    {
        return m_alignment_size;
    }
    VkBufferUsageFlags Buffer::getUsageFlags() const
    {
        return m_usage_flags;
    }
    VkMemoryPropertyFlags Buffer::getMemoryPropertyFlags() const
    {
        return m_memory_property_flags;
    }
    VkDeviceSize Buffer::getBufferSize() const
    {
        return m_buffer_size;
    }
    VkDeviceSize Buffer::getAlignment(VkDeviceSize instance_size, VkDeviceSize min_offset_alignment)
    {
        if (min_offset_alignment > 0)
        {
            return (instance_size + min_offset_alignment - 1) & ~(min_offset_alignment - 1);
        }
        return instance_size;
    }
}
