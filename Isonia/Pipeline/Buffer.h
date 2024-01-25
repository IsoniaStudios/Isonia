#pragma once

#include "Device.h"

#include <cassert>
#include <cstring>

namespace Isonia::Pipeline
{
	class Buffer
	{
	public:
		Buffer(
			Device& device,
			VkDeviceSize instanceSize,
			uint32_t instanceCount,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryPropertyFlags,
			VkDeviceSize minOffsetAlignment = 1
		) : device(device),
			instanceSize(instanceSize),
			instanceCount(instanceCount),
			usageFlags(usageFlags),
			memoryPropertyFlags(memoryPropertyFlags)
		{
			alignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
			bufferSize = alignmentSize * instanceCount;
			device.CreateBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
		}

		~Buffer()
		{
			Unmap();
			vkDestroyBuffer(device.GetDevice(), buffer, nullptr);
			vkFreeMemory(device.GetDevice(), memory, nullptr);
		}

		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		/**
		 * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
		 *
		 * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
		 * buffer range.
		 * @param offset (Optional) Byte offset from beginning
		 *
		 * @return VkResult of the buffer mapping call
		 */
		VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
		{
			assert(buffer && memory && "Called map on buffer before create");
			return vkMapMemory(device.GetDevice(), memory, offset, size, 0, &mapped);
		}

		/**
		 * Unmap a mapped memory range
		 *
		 * @note Does not return a result as vkUnmapMemory can't fail
		 */
		void Unmap()
		{
			if (mapped)
			{
				vkUnmapMemory(device.GetDevice(), memory);
				mapped = nullptr;
			}
		}

		/**
		 * Copies the specified data to the mapped buffer. Default value writes whole buffer range
		 *
		 * @param data Pointer to the data to copy
		 * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
		 * range.
		 * @param offset (Optional) Byte offset from beginning of mapped region
		 *
		 */
		void WriteToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
		{
			assert(mapped && "Cannot copy to unmapped buffer");

			if (size == VK_WHOLE_SIZE)
			{
				memcpy(mapped, data, bufferSize);
			}
			else
			{
				char* memOffset = (char*)mapped;
				memOffset += offset;
				memcpy(memOffset, data, size);
			}
		}

		/**
		 * Flush a memory range of the buffer to make it visible to the device
		 *
		 * @note Only required for non-coherent memory
		 *
		 * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
		 * complete buffer range.
		 * @param offset (Optional) Byte offset from beginning
		 *
		 * @return VkResult of the flush call
		 */
		VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
		{
			VkMappedMemoryRange mappedRange = {};
			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mappedRange.memory = memory;
			mappedRange.offset = offset;
			mappedRange.size = size;
			return vkFlushMappedMemoryRanges(device.GetDevice(), 1, &mappedRange);
		}

		/**
		 * Invalidate a memory range of the buffer to make it visible to the host
		 *
		 * @note Only required for non-coherent memory
		 *
		 * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
		 * the complete buffer range.
		 * @param offset (Optional) Byte offset from beginning
		 *
		 * @return VkResult of the invalidate call
		 */
		VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
		{
			VkMappedMemoryRange mappedRange = {};
			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mappedRange.memory = memory;
			mappedRange.offset = offset;
			mappedRange.size = size;
			return vkInvalidateMappedMemoryRanges(device.GetDevice(), 1, &mappedRange);
		}

		/**
		 * Create a buffer info descriptor
		 *
		 * @param size (Optional) Size of the memory range of the descriptor
		 * @param offset (Optional) Byte offset from beginning
		 *
		 * @return VkDescriptorBufferInfo of specified offset and range
		 */
		VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
		{
			return VkDescriptorBufferInfo{
				buffer,
				offset,
				size,
			};
		}

		/**
		 * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
		 *
		 * @param data Pointer to the data to copy
		 * @param index Used in offset calculation
		 *
		 */
		void WriteToIndex(void* data, int index)
		{
			WriteToBuffer(data, instanceSize, index * alignmentSize);
		}

		/**
		 * Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
		 *
		 * @param index Used in offset calculation
		 *
		 */
		VkResult FlushIndex(int index)
		{
			return Flush(alignmentSize, index * alignmentSize);
		}

		/**
		 * Create a buffer info descriptor
		 *
		 * @param index Specifies the region given by index * alignmentSize
		 *
		 * @return VkDescriptorBufferInfo for instance at index
		 */
		VkDescriptorBufferInfo DescriptorInfoForIndex(int index)
		{
			return DescriptorInfo(alignmentSize, index * alignmentSize);
		}

		/**
		 * Invalidate a memory range of the buffer to make it visible to the host
		 *
		 * @note Only required for non-coherent memory
		 *
		 * @param index Specifies the region to invalidate: index * alignmentSize
		 *
		 * @return VkResult of the invalidate call
		 */
		VkResult InvalidateIndex(int index)
		{
			return Invalidate(alignmentSize, index * alignmentSize);
		}

		VkBuffer GetBuffer() const { return buffer; }
		void* GetMappedMemory() const { return mapped; }
		uint32_t GetInstanceCount() const { return instanceCount; }
		VkDeviceSize GetInstanceSize() const { return instanceSize; }
		VkDeviceSize GetAlignmentSize() const { return instanceSize; }
		VkBufferUsageFlags GetUsageFlags() const { return usageFlags; }
		VkMemoryPropertyFlags GetMemoryPropertyFlags() const { return memoryPropertyFlags; }
		VkDeviceSize GetBufferSize() const { return bufferSize; }

	private:
		/**
		 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
		 *
		 * @param instanceSize The size of an instance
		 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
		 * minUniformBufferOffsetAlignment)
		 *
		 * @return VkResult of the buffer mapping call
		 */
		static VkDeviceSize GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
		{
			if (minOffsetAlignment > 0)
			{
				return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
			}
			return instanceSize;
		}

		Device& device;
		void* mapped = nullptr;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;

		VkDeviceSize bufferSize;
		uint32_t instanceCount;
		VkDeviceSize instanceSize;
		VkDeviceSize alignmentSize;
		VkBufferUsageFlags usageFlags;
		VkMemoryPropertyFlags memoryPropertyFlags;
	};
}
