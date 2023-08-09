#pragma once
#include <vulkan/vulkan.hpp>
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
				class AIR_ENGINE_API Buffer final
				{
				private:
					vk::Buffer _vkBuffer;
					vk::DeviceSize _size;
					vk::BufferUsageFlags _usage;
					std::shared_ptr<Memory> _memory;
				public:
					Buffer(vk::DeviceSize size, vk::BufferUsageFlags bufferUsage, vk::MemoryPropertyFlags property, VmaAllocationCreateFlags flags = 0, VmaMemoryUsage memoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO);
					Buffer(vk::DeviceSize size, vk::BufferUsageFlags bufferUsage, std::shared_ptr<Memory> memory);
					Buffer(vk::DeviceSize size, vk::BufferUsageFlags bufferUsage);
					~Buffer();
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