#pragma once
#include <vulkan/vulkan_core.h>
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"
#include <memory>
#include "Memory.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Memory;
				class DLL_API Buffer final
				{
				private:
					VkBuffer _vkBuffer;
					VkDeviceSize _size;
					VkBufferUsageFlags _usage;
					std::shared_ptr<Memory> _memory;
				public:
					Buffer(VkDeviceSize size, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags property, VmaAllocationCreateFlags flags = 0, VmaMemoryUsage memoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO);
					Buffer(VkDeviceSize size, VkBufferUsageFlags bufferUsage, std::shared_ptr<Memory> memory);
					Buffer(VkDeviceSize size, VkBufferUsageFlags bufferUsage);
					~Buffer();
					inline VkBuffer VkHandle() const
					{
						return _vkBuffer;
					}
					inline VkDeviceSize Size() const
					{
						return _size;
					}
					inline VkDeviceSize Offset() const
					{
						return 0;
					}
					inline void SetMemory(std::shared_ptr<Memory> memory)
					{
						_memory = std::move(memory);
						auto bindResult = vmaBindBufferMemory(Core::Manager::GraphicDeviceManager::VmaAllocator(), _memory->Allocation(), _vkBuffer);
						if (VK_SUCCESS != bindResult) qFatal("Failed to bind buffer.");
					}
					inline std::shared_ptr<Memory> Memory()const
					{
						return _memory;
					}
					void ClearMemory()
					{
						_memory.reset();
					}

					NO_COPY_MOVE(Buffer)
				};
			}
		}
	}
}