#include "DescriptorManager.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"
#include "../Instance/Buffer.hpp"

std::map<uint32_t, AirEngine::Runtime::Graphic::Manager::DescriptorManager::DescriptorMemoryHandle> AirEngine::Runtime::Graphic::Manager::DescriptorManager::_freeMemoryMap{};
size_t AirEngine::Runtime::Graphic::Manager::DescriptorManager::_currentSize{};
uint16_t AirEngine::Runtime::Graphic::Manager::DescriptorManager::_descriptorMemoryAlignment{};
uint8_t AirEngine::Runtime::Graphic::Manager::DescriptorManager::_descriptorMemoryAlignmentStride{};

void AirEngine::Runtime::Graphic::Manager::DescriptorManager::CreateHostMemory(size_t size)
{
	delete _hostMemory;

	_hostMemory = reinterpret_cast<uint8_t*>(std::malloc(size));
}

inline void AirEngine::Runtime::Graphic::Manager::DescriptorManager::IncreaseHostMemory()
{
	DescriptorMemoryHandle newHandle(ToCompressed(_currentSize), ToCompressed(_currentSize));

	bool isMerged = false;
	if (_freeMemoryMap.size())
	{
		auto& lastHandle = _freeMemoryMap.rbegin()->second;
		if (lastHandle.offset + lastHandle.size == newHandle.offset)
		{
			lastHandle.size = lastHandle.size + ToCompressed(_currentSize);
			isMerged = true;
		}
	}

	if(!isMerged)
	{
		_freeMemoryMap.emplace(newHandle);
	}

	_currentSize *= 2;
	CreateHostMemory(_currentSize);
}

AirEngine::Runtime::Graphic::Manager::DescriptorManager::DescriptorMemoryHandle AirEngine::Runtime::Graphic::Manager::DescriptorManager::AllocateDescriptorMemory(size_t size)
{
	const auto&& alignedSize = ToAligned(size);
	const auto&& compressedSize = ToCompressed(alignedSize);

	DescriptorMemoryHandle outHandle{};
	bool isLaden = false;
	{
		for (auto iter = _freeMemoryMap.cbegin(); iter != _freeMemoryMap.cend(); ++iter)
		{
			const auto& freeMemoryHandle = iter->second;
			if (compressedSize <= freeMemoryHandle.size)
			{
				outHandle.offset = freeMemoryHandle.offset;
				outHandle.size = compressedSize;

				DescriptorMemoryHandle newFreeMemoryHandle(freeMemoryHandle.offset + compressedSize, freeMemoryHandle.size - compressedSize);
				isLaden = false;
				_freeMemoryMap.erase(iter);
				if (newFreeMemoryHandle.size > 0)
				{
					_freeMemoryMap.emplace(newFreeMemoryHandle);
				}
			}
		}
		isLaden = true;
	}

	if (!isLaden)
	{
		return outHandle;
	}

	IncreaseHostMemory();

	return AllocateDescriptorMemory(size);
}

AirEngine::Runtime::Graphic::Manager::DescriptorManager::DescriptorMemoryHandle AirEngine::Runtime::Graphic::Manager::DescriptorManager::ReallocateDescriptorMemory(size_t size)
{
	return DescriptorMemoryHandle();
}

AirEngine::Runtime::Graphic::Manager::DescriptorManager::DescriptorMemoryHandle AirEngine::Runtime::Graphic::Manager::DescriptorManager::FreeDescriptorMemory(size_t size)
{
	return DescriptorMemoryHandle();
}

void AirEngine::Runtime::Graphic::Manager::DescriptorManager::Initialize()
{
	auto&& physicalDeviceDescriptorBufferPropertiesEXT = vk::PhysicalDeviceDescriptorBufferPropertiesEXT();

	auto&& physicalDeviceProperties = vk::PhysicalDeviceProperties2();
	physicalDeviceProperties.pNext = &physicalDeviceDescriptorBufferPropertiesEXT;

	Core::Manager::GraphicDeviceManager::PhysicalDevice().getProperties2(&physicalDeviceProperties);

	_descriptorMemoryAlignment = physicalDeviceDescriptorBufferPropertiesEXT.descriptorBufferOffsetAlignment;
	_descriptorMemoryAlignmentStride = std::log2(_descriptorMemoryAlignment);

	_currentSize = 4 * 1024 * 1024;
	CreateHostMemory(_currentSize);
}
