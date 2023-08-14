#include "DescriptorManager.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"
#include "../Instance/Buffer.hpp"

std::map<uint32_t, AirEngine::Runtime::Graphic::Manager::DescriptorManager::DescriptorMemoryHandle> AirEngine::Runtime::Graphic::Manager::DescriptorManager::_freeMemoryMap{};
size_t AirEngine::Runtime::Graphic::Manager::DescriptorManager::_currentSize{};
uint16_t AirEngine::Runtime::Graphic::Manager::DescriptorManager::_descriptorMemoryAlignment{};
uint8_t AirEngine::Runtime::Graphic::Manager::DescriptorManager::_descriptorMemoryAlignmentStride{};
AirEngine::Runtime::Graphic::Instance::Buffer* AirEngine::Runtime::Graphic::Manager::DescriptorManager::_deviceBuffer = nullptr;
AirEngine::Runtime::Graphic::Instance::Buffer* AirEngine::Runtime::Graphic::Manager::DescriptorManager::_hostCachedBuffer = nullptr;
uint8_t* AirEngine::Runtime::Graphic::Manager::DescriptorManager::_hostMemory = nullptr;

void AirEngine::Runtime::Graphic::Manager::DescriptorManager::CreateHostMemory(size_t size)
{
	std::free(_hostMemory);

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
			isMerged = true;
			
			lastHandle.size = lastHandle.size + newHandle.size;
		}
	}

	if(!isMerged)
	{
		_freeMemoryMap.emplace(newHandle.offset, newHandle);
	}

	_currentSize *= 2;
	CreateHostMemory(_currentSize);
}

AirEngine::Runtime::Graphic::Manager::DescriptorManager::DescriptorMemoryHandle AirEngine::Runtime::Graphic::Manager::DescriptorManager::AllocateDescriptorMemory(size_t size)
{
	const auto&& alignedSize = ToAligned(size);
	const auto&& compressedSize = ToCompressed(alignedSize);

	DescriptorMemoryHandle outHandle{};
	bool isLaden = true;
	{
		for (auto iter = _freeMemoryMap.cbegin(); iter != _freeMemoryMap.cend(); ++iter)
		{
			const auto& freeMemoryHandle = iter->second;
			if (compressedSize <= freeMemoryHandle.size)
			{
				isLaden = false;

				outHandle.offset = freeMemoryHandle.offset;
				outHandle.size = compressedSize;

				DescriptorMemoryHandle newFreeMemoryHandle(freeMemoryHandle.offset + compressedSize, freeMemoryHandle.size - compressedSize);
				_freeMemoryMap.erase(iter);
				if (newFreeMemoryHandle.size > 0)
				{
					_freeMemoryMap.emplace(newFreeMemoryHandle.offset, newFreeMemoryHandle);
				}

				break;
			}
		}
	}

	if (isLaden)
	{
		IncreaseHostMemory();
		outHandle = AllocateDescriptorMemory(size);
	}

	return outHandle;
}

AirEngine::Runtime::Graphic::Manager::DescriptorManager::DescriptorMemoryHandle AirEngine::Runtime::Graphic::Manager::DescriptorManager::ReallocateDescriptorMemory(size_t size)
{
	return DescriptorMemoryHandle();
}

void AirEngine::Runtime::Graphic::Manager::DescriptorManager::FreeDescriptorMemory(DescriptorMemoryHandle descriptorMemoryHandle)
{
	auto&& rightIter = _freeMemoryMap.upper_bound(descriptorMemoryHandle.offset);
	if (rightIter != _freeMemoryMap.end())
	{
		auto leftIter = rightIter;
		--leftIter;
		bool isLeftMerged = false;
		if (leftIter != _freeMemoryMap.end())
		{
			auto& leftHandle = leftIter->second;
			if (leftHandle.offset + leftHandle.size == descriptorMemoryHandle.offset)
			{
				isLeftMerged = true;

				leftHandle.size += descriptorMemoryHandle.size;
			}
		}
		
		auto& rightHandle = rightIter->second;
		if (descriptorMemoryHandle.offset + descriptorMemoryHandle.size == rightHandle.offset)
		{
			if (isLeftMerged)
			{
				auto& leftHandle = leftIter->second;
				leftHandle.size += rightHandle.size;
			}
			else
			{
				DescriptorMemoryHandle newHandle(descriptorMemoryHandle.offset, descriptorMemoryHandle.size + rightHandle.size);
				_freeMemoryMap.emplace(newHandle.offset, newHandle);
			}
			_freeMemoryMap.erase(rightIter);

			return;
		}
	}
	else
	{
		auto&& leftIter = _freeMemoryMap.rbegin();
		if (leftIter != _freeMemoryMap.rend())
		{
			auto& leftHandle = leftIter->second;
			if (leftHandle.offset + leftHandle.size == descriptorMemoryHandle.offset)
			{
				leftHandle.size += descriptorMemoryHandle.size;

				return;
			}
		}
	}

	_freeMemoryMap.emplace(descriptorMemoryHandle.offset, descriptorMemoryHandle);
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
	DescriptorMemoryHandle newHandle(0, ToCompressed(_currentSize));
	_freeMemoryMap.emplace(newHandle.offset, newHandle);
}
