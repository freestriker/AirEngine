#pragma once
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include <map>
#include <vulkan/vulkan.hpp>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Manager
			{
				class GraphicDeviceManager;
			}
		}
		namespace Graphic
		{
			namespace Instance
			{
				class Buffer;
			}
			namespace Manager
			{
				class AIR_ENGINE_API DescriptorManager final
				{
					friend class Core::Manager::GraphicDeviceManager;
				public:
					struct DescriptorMemoryHandle
					{
						uint32_t offset;
						uint32_t size;
					};
				private:
					DescriptorManager() = delete;
					~DescriptorManager() = delete;
					NO_COPY_MOVE(DescriptorManager)
				private:
					static std::map<uint32_t, DescriptorMemoryHandle> _freeMemoryMap;
					static size_t _currentSize;
					static uint16_t _descriptorMemoryAlignment;
					static uint8_t _descriptorMemoryAlignmentStride;
					static Instance::Buffer* _deviceBuffer;
					static Instance::Buffer* _hostCachedBuffer;
					static uint8_t* _hostMemory;

					static void Initialize();
				public:
					static inline size_t ToAligned(size_t size)
					{
						return (size + _descriptorMemoryAlignment - 1) & ~(_descriptorMemoryAlignment - 1);
					}
					static inline size_t ToCompressed(size_t size)
					{
						return size >> _descriptorMemoryAlignmentStride;
					}
					static inline size_t ToAlignedCompressed(size_t size)
					{
						return ToCompressed(ToAligned(size));
					}
					static void CreateHostMemory(size_t size);
					static void IncreaseHostMemory();
					static DescriptorMemoryHandle AllocateDescriptorMemory(size_t size);
					static DescriptorMemoryHandle ReallocateDescriptorMemory(size_t size);
					static DescriptorMemoryHandle FreeDescriptorMemory(size_t size);
				private:
				public:
					inline static uint16_t DescriptorMemoryAlignment()
					{
						return _descriptorMemoryAlignment;
					}
				};
			}
		}
	}
}