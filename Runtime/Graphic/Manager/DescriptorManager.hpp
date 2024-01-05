#pragma once
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include <map>
#include <vulkan/vulkan.hpp>
#include "DescriptorManagerData.hpp"
#include "AirEngine/Runtime/Utility/ThreadInclude.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Manager
			{
				class RenderManager;
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
					friend class Core::Manager::RenderManager;
				private:
					DescriptorManager() = delete;
					~DescriptorManager() = delete;
					NO_COPY_MOVE(DescriptorManager)

					static void Initialize();
				private:
					static std::mutex _mutex;
					static std::map<uint32_t, DescriptorMemoryHandle> _freeMemoryMap;
					static size_t _currentSize;
					static uint16_t _descriptorMemoryAlignment;
					static uint8_t _descriptorMemoryAlignmentStride;
					static Instance::Buffer* _deviceBuffer;
					static Instance::Buffer* _hostCachedBuffer;
					static std::vector<uint8_t> _hostMemory;
					static std::vector<DescriptorMemoryHandle> _dirtyHandles;
				public:
					static inline std::mutex& Mutex()
					{
						return _mutex;
					}
					static inline size_t ToAligned(size_t size)
					{
						return (size + _descriptorMemoryAlignment - 1) & ~(_descriptorMemoryAlignment - 1);
					}
					static inline size_t ToCompressed(size_t size)
					{
						return size >> _descriptorMemoryAlignmentStride;
					}
					static inline size_t FromCompressed(size_t size)
					{
						return size << _descriptorMemoryAlignmentStride;
					}
					static inline size_t ToAlignedCompressed(size_t size)
					{
						return ToCompressed(ToAligned(size));
					}
					static inline const std::vector<uint8_t>& HostMemory()
					{
						return _hostMemory;
					}

					static void IncreaseHostMemory();

					static DescriptorMemoryHandle AllocateDescriptorMemory(size_t size);
					static DescriptorMemoryHandle ReallocateDescriptorMemory(DescriptorMemoryHandle descriptorMemoryHandle, size_t size);
					static void FreeDescriptorMemory(DescriptorMemoryHandle descriptorMemoryHandle);

					static void WriteToHostDescriptorMemory(DescriptorMemoryHandle descriptorMemoryHandle, uint8_t* dataPtr, uint32_t offset, uint32_t size);
					static void ClearHostDescriptorMemory(DescriptorMemoryHandle descriptorMemoryHandle, uint32_t offset, uint32_t size);

					static std::vector<DescriptorMemoryHandle> MergeAndClearDirtyHandles();
					static void CopyHostDirtyDataToCachedBuffer(const std::vector<DescriptorMemoryHandle>& dirtyHandles);
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