#pragma once
#include <vulkan/vulkan.hpp>
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include <vk_mem_alloc.h>
#include "AirEngine/Runtime/Core/Manager/GraphicDeviceManager.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class AIR_ENGINE_API Memory final
				{
					friend class Buffer;
					friend class Image;
				private:
					const VmaAllocation _vmaAllocation;
					const uint32_t _memoryType;
					const vk::DeviceMemory _vkDeviceMemory;
					const vk::DeviceSize _offset;
					const vk::DeviceSize _size;
				private:
					Memory(VmaAllocation vmaAllocation, const VmaAllocationInfo& vmaInfo);
				public:
					~Memory();
					NO_COPY_MOVE(Memory)
					
					inline vk::DeviceMemory VkHandle()const
					{
						return _vkDeviceMemory;
					}
					inline vk::DeviceSize Size()const
					{
						return _size;
					}
					inline vk::DeviceSize Offset()const
					{
						return _offset;
					}
					inline void* Map()const
					{
						void* ptr = nullptr;
						auto mapResult = vmaMapMemory(Core::Manager::GraphicDeviceManager::VmaAllocator(), _vmaAllocation, &ptr);
						if (mapResult != VK_SUCCESS) qFatal("Failed to map memory.");
						return ptr;
					}
					inline void Flush(vk::DeviceSize offset, vk::DeviceSize size) const
					{
						vmaFlushAllocation(
							Core::Manager::GraphicDeviceManager::VmaAllocator(),
							_vmaAllocation,
							offset,
							size
						);
					}
					inline void Flush() const
					{
						Flush(_offset, _size);
					}
					inline void Unmap()const
					{
						vmaUnmapMemory(Core::Manager::GraphicDeviceManager::VmaAllocator(), _vmaAllocation);
					}
					inline VmaAllocation Allocation()const
					{
						return _vmaAllocation;
					}
				};
			}
		}
	}
}