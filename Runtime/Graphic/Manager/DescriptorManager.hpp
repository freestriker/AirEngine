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
						friend class DescriptorManager;
					private:
						uint32_t offset;
						uint32_t size;
						DescriptorMemoryHandle(uint32_t offset, uint32_t size)
							: offset(offset)
							, size(size)
						{

						}
						DescriptorMemoryHandle()
							: offset(0)
							, size(0)
						{

						}
					public:
						inline size_t Offset()const;
						inline size_t Size()const;
					};
				private:
					DescriptorManager() = delete;
					~DescriptorManager() = delete;
					NO_COPY_MOVE(DescriptorManager)

					static void Initialize();
				private:
					static std::map<uint32_t, DescriptorMemoryHandle> _freeMemoryMap;
					static size_t _currentSize;
					static uint16_t _descriptorMemoryAlignment;
					static uint8_t _descriptorMemoryAlignmentStride;
					static Instance::Buffer* _deviceBuffer;
					static Instance::Buffer* _hostCachedBuffer;
					static uint8_t* _hostMemory;
				public:
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

					static void IncreaseHostMemory();

					static DescriptorMemoryHandle AllocateDescriptorMemory(size_t size);
					static DescriptorMemoryHandle ReallocateDescriptorMemory(DescriptorMemoryHandle descriptorMemoryHandle, size_t size);
					static void FreeDescriptorMemory(DescriptorMemoryHandle descriptorMemoryHandle);

					static void WriteToHostDescriptorMemory(DescriptorMemoryHandle descriptorMemoryHandle, uint8_t* dataPtr, uint32_t offset, uint32_t size);
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

inline size_t AirEngine::Runtime::Graphic::Manager::DescriptorManager::DescriptorMemoryHandle::Offset() const
{
	return AirEngine::Runtime::Graphic::Manager::DescriptorManager::FromCompressed(offset);
}

inline size_t AirEngine::Runtime::Graphic::Manager::DescriptorManager::DescriptorMemoryHandle::Size() const
{
	return AirEngine::Runtime::Graphic::Manager::DescriptorManager::FromCompressed(size);
}
