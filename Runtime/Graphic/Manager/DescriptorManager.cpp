#include "DescriptorManager.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"
#include "../Instance/Buffer.hpp"

std::map<uint32_t, AirEngine::Runtime::Graphic::Manager::DescriptorMemoryHandle> AirEngine::Runtime::Graphic::Manager::DescriptorManager::_freeMemoryMap{};
size_t AirEngine::Runtime::Graphic::Manager::DescriptorManager::_currentSize{};
uint16_t AirEngine::Runtime::Graphic::Manager::DescriptorManager::_descriptorMemoryAlignment{};
uint8_t AirEngine::Runtime::Graphic::Manager::DescriptorManager::_descriptorMemoryAlignmentStride{};
AirEngine::Runtime::Graphic::Instance::Buffer* AirEngine::Runtime::Graphic::Manager::DescriptorManager::_deviceBuffer = nullptr;
AirEngine::Runtime::Graphic::Instance::Buffer* AirEngine::Runtime::Graphic::Manager::DescriptorManager::_hostCachedBuffer = nullptr;
uint8_t* AirEngine::Runtime::Graphic::Manager::DescriptorManager::_hostMemory = nullptr;

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

	{
		auto&& newHostMemory = reinterpret_cast<uint8_t*>(std::malloc(_currentSize));
		if (newHostMemory == nullptr)
		{
			qFatal("Allocate memory failed.");
		}

		std::memcpy(newHostMemory, _hostMemory, _currentSize >> 2);
		std::memset(newHostMemory + (_currentSize >> 2), 0, _currentSize >> 2);

		std::free(_hostMemory);
		_hostMemory = newHostMemory;
	}
}

AirEngine::Runtime::Graphic::Manager::DescriptorMemoryHandle AirEngine::Runtime::Graphic::Manager::DescriptorManager::AllocateDescriptorMemory(size_t size)
{
	if (size == 0)
	{
		qFatal("Memory size must greater than 0.");
	}

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

AirEngine::Runtime::Graphic::Manager::DescriptorMemoryHandle AirEngine::Runtime::Graphic::Manager::DescriptorManager::ReallocateDescriptorMemory(DescriptorMemoryHandle descriptorMemoryHandle, size_t size)
{
	if (size == 0)
	{
		qFatal("Memory size must greater than 0.");
	}

	if ((descriptorMemoryHandle.offset + descriptorMemoryHandle.size) << _descriptorMemoryAlignmentStride > _currentSize || descriptorMemoryHandle.size == 0)
	{
		qFatal("Memory handle is not valid.");
	}

	const auto&& alignedSize = ToAligned(size);
	const auto&& compressedSize = ToCompressed(alignedSize);

	if (descriptorMemoryHandle.size == compressedSize)
	{
		return descriptorMemoryHandle;
	}

	if (compressedSize < descriptorMemoryHandle.size)
	{
		DescriptorMemoryHandle freeHandle(descriptorMemoryHandle.offset + compressedSize, descriptorMemoryHandle.size - compressedSize);
		FreeDescriptorMemory(freeHandle);

		return 	DescriptorMemoryHandle(descriptorMemoryHandle.offset, compressedSize);
	}

	auto&& rightIter = _freeMemoryMap.upper_bound(descriptorMemoryHandle.offset);
	if (rightIter != _freeMemoryMap.end())
	{
		auto& rightHandle = rightIter->second;
		if (descriptorMemoryHandle.offset + descriptorMemoryHandle.size == rightHandle.offset)
		{
			if (compressedSize - descriptorMemoryHandle.size == rightHandle.size)
			{
				_freeMemoryMap.erase(rightIter);
				return DescriptorMemoryHandle(descriptorMemoryHandle.offset, compressedSize);
			}
			else if (compressedSize - descriptorMemoryHandle.size < rightHandle.size)
			{
				DescriptorMemoryHandle newHandle(rightHandle.offset + (compressedSize - descriptorMemoryHandle.size), rightHandle.size - (compressedSize - descriptorMemoryHandle.size));
				_freeMemoryMap.emplace(newHandle.offset, newHandle);
				_freeMemoryMap.erase(rightIter);
				return DescriptorMemoryHandle(descriptorMemoryHandle.offset, compressedSize);
			}
		}
	}

	FreeDescriptorMemory(descriptorMemoryHandle);
	return AllocateDescriptorMemory(size);
}

void AirEngine::Runtime::Graphic::Manager::DescriptorManager::FreeDescriptorMemory(DescriptorMemoryHandle descriptorMemoryHandle)
{
	if ((descriptorMemoryHandle.offset + descriptorMemoryHandle.size) << _descriptorMemoryAlignmentStride > _currentSize || descriptorMemoryHandle.size == 0)
	{
		qFatal("Memory handle is not valid.");
	}

	std::memset(_hostMemory + descriptorMemoryHandle.Offset(), 0, descriptorMemoryHandle.Size());

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

void AirEngine::Runtime::Graphic::Manager::DescriptorManager::WriteToHostDescriptorMemory(DescriptorMemoryHandle descriptorMemoryHandle, uint8_t* dataPtr, uint32_t offset, uint32_t size)
{
	if ((descriptorMemoryHandle.offset + descriptorMemoryHandle.size) << _descriptorMemoryAlignmentStride > _currentSize || descriptorMemoryHandle.size == 0)
	{
		qFatal("Memory handle is not valid.");
	}

	size_t blockOffset = FromCompressed(descriptorMemoryHandle.offset);
	size_t blockSize = FromCompressed(descriptorMemoryHandle.size);
	
	if (size == 0)
	{
		qFatal("Can not write zero size data.");
	}

	if (offset + size >= blockSize)
	{
		qFatal("Data size is bigger than block.");
	}

	std::memcpy(_hostMemory + blockOffset + offset, dataPtr, size);
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
	_hostMemory = reinterpret_cast<uint8_t*>(std::malloc(_currentSize));
	std::memset(_hostMemory, 0, _currentSize);
	DescriptorMemoryHandle newHandle(0, ToCompressed(_currentSize));
	_freeMemoryMap.emplace(newHandle.offset, newHandle);
}