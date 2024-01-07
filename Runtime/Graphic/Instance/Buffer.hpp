#pragma once
#include <vulkan/vulkan.hpp>
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"
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
				class AIR_ENGINE_API Buffer
				{
				protected:
					vk::Buffer _vkBuffer;
					vk::DeviceSize _size;
					vk::BufferUsageFlags _usage;
					std::shared_ptr<Memory> _memory;
					uint64_t _bufferAddress;
				public:
					Buffer(vk::DeviceSize size, vk::BufferUsageFlags bufferUsage, vk::MemoryPropertyFlags property, VmaAllocationCreateFlags flags = 0, VmaMemoryUsage memoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO);
					Buffer(vk::DeviceSize size, vk::BufferUsageFlags bufferUsage, std::shared_ptr<Memory> memory);
					Buffer(vk::DeviceSize size, vk::BufferUsageFlags bufferUsage);
					virtual ~Buffer();
					inline vk::Buffer VkHandle() const
					{
						return _vkBuffer;
					}
					inline vk::DeviceSize Size() const
					{
						return _size;
					}
					inline vk::DeviceSize Offset() const
					{
						return 0;
					}
					inline void SetMemory(std::shared_ptr<Memory> memory)
					{
						_memory = std::move(memory);
						auto bindResult = vmaBindBufferMemory(Graphic::Manager::DeviceManager::VmaAllocator(), _memory->Allocation(), _vkBuffer);
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
					uint64_t BufferDeviceAddress()const
					{
						return _bufferAddress;
					}

					NO_COPY_MOVE(Buffer)
				};
			}
		}
	}
}