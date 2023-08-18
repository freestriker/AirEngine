#include "DescriptorManager.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"
#include "../Instance/Buffer.hpp"

std::map<uint32_t, AirEngine::Runtime::Graphic::Manager::DescriptorMemoryHandle> AirEngine::Runtime::Graphic::Manager::DescriptorManager::_freeMemoryMap{};
size_t AirEngine::Runtime::Graphic::Manager::DescriptorManager::_currentSize{};
uint16_t AirEngine::Runtime::Graphic::Manager::DescriptorManager::_descriptorMemoryAlignment{};
uint8_t AirEngine::Runtime::Graphic::Manager::DescriptorManager::_descriptorMemoryAlignmentStride{};
AirEngine::Runtime::Graphic::Instance::Buffer* AirEngine::Runtime::Graphic::Manager::DescriptorManager::_deviceBuffer = nullptr;
AirEngine::Runtime::Graphic::Instance::Buffer* AirEngine::Runtime::Graphic::Manager::DescriptorManager::_hostCachedBuffer = nullptr;
std::vector<uint8_t> AirEngine::Runtime::Graphic::Manager::DescriptorManager::_hostMemory{};

inline void AirEngine::Runtime::Graphic::Manager::DescriptorManager::IncreaseHostMemory()
{
	DescriptorMemoryHandle newHandle(ToCompressed(_currentSize), ToCompressed(_currentSize));

	bool isMerged = false;
	if (_freeMemoryMap.size())
	{
		auto& lastHandle = _freeMemoryMap.rbegin()->second;
		if (lastHandle.compressedOffset + lastHandle.compressedSize == newHandle.compressedOffset)
		{
			isMerged = true;
			
			lastHandle.compressedSize = lastHandle.compressedSize + newHandle.compressedSize;
		}
	}

	if(!isMerged)
	{
		_freeMemoryMap.emplace(newHandle.compressedOffset, newHandle);
	}

	_currentSize *= 2;

	{
		auto&& newHostMemory = reinterpret_cast<uint8_t*>(std::malloc(_currentSize));
		if (newHostMemory == nullptr)
		{
			qFatal("Allocate memory failed.");
		}

		_hostMemory.resize(_currentSize, 0);
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
			if (compressedSize <= freeMemoryHandle.compressedSize)
			{
				isLaden = false;

				outHandle.compressedOffset = freeMemoryHandle.compressedOffset;
				outHandle.compressedSize = compressedSize;

				DescriptorMemoryHandle newFreeMemoryHandle(freeMemoryHandle.compressedOffset + compressedSize, freeMemoryHandle.compressedSize - compressedSize);
				_freeMemoryMap.erase(iter);
				if (newFreeMemoryHandle.compressedSize > 0)
				{
					_freeMemoryMap.emplace(newFreeMemoryHandle.compressedOffset, newFreeMemoryHandle);
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

	if ((descriptorMemoryHandle.compressedOffset + descriptorMemoryHandle.compressedSize) << _descriptorMemoryAlignmentStride > _currentSize || descriptorMemoryHandle.compressedSize == 0)
	{
		qFatal("Memory handle is not valid.");
	}

	const auto&& alignedSize = ToAligned(size);
	const auto&& compressedSize = ToCompressed(alignedSize);

	if (descriptorMemoryHandle.compressedSize == compressedSize)
	{
		return descriptorMemoryHandle;
	}

	if (compressedSize < descriptorMemoryHandle.compressedSize)
	{
		DescriptorMemoryHandle freeHandle(descriptorMemoryHandle.compressedOffset + compressedSize, descriptorMemoryHandle.compressedSize - compressedSize);
		FreeDescriptorMemory(freeHandle);

		return 	DescriptorMemoryHandle(descriptorMemoryHandle.compressedOffset, compressedSize);
	}

	auto&& rightIter = _freeMemoryMap.upper_bound(descriptorMemoryHandle.compressedOffset);
	if (rightIter != _freeMemoryMap.end())
	{
		auto& rightHandle = rightIter->second;
		if (descriptorMemoryHandle.compressedOffset + descriptorMemoryHandle.compressedSize == rightHandle.compressedOffset)
		{
			if (compressedSize - descriptorMemoryHandle.compressedSize == rightHandle.compressedSize)
			{
				_freeMemoryMap.erase(rightIter);
				return DescriptorMemoryHandle(descriptorMemoryHandle.compressedOffset, compressedSize);
			}
			else if (compressedSize - descriptorMemoryHandle.compressedSize < rightHandle.compressedSize)
			{
				DescriptorMemoryHandle newHandle(rightHandle.compressedOffset + (compressedSize - descriptorMemoryHandle.compressedSize), rightHandle.compressedSize - (compressedSize - descriptorMemoryHandle.compressedSize));
				_freeMemoryMap.emplace(newHandle.compressedOffset, newHandle);
				_freeMemoryMap.erase(rightIter);
				return DescriptorMemoryHandle(descriptorMemoryHandle.compressedOffset, compressedSize);
			}
		}
	}

	auto&& newHandle = AllocateDescriptorMemory(size);
	std::memcpy(_hostMemory.data() + newHandle.Offset(), _hostMemory.data() + descriptorMemoryHandle.Offset(), descriptorMemoryHandle.Size());
	FreeDescriptorMemory(descriptorMemoryHandle);

	return newHandle;
}

void AirEngine::Runtime::Graphic::Manager::DescriptorManager::FreeDescriptorMemory(DescriptorMemoryHandle descriptorMemoryHandle)
{
	if ((descriptorMemoryHandle.compressedOffset + descriptorMemoryHandle.compressedSize) << _descriptorMemoryAlignmentStride > _currentSize || descriptorMemoryHandle.compressedSize == 0)
	{
		qFatal("Memory handle is not valid.");
	}

	std::memset(_hostMemory.data() + descriptorMemoryHandle.Offset(), 0, descriptorMemoryHandle.Size());

	auto&& rightIter = _freeMemoryMap.upper_bound(descriptorMemoryHandle.compressedOffset);
	if (rightIter != _freeMemoryMap.end())
	{
		auto leftIter = rightIter;
		--leftIter;
		bool isLeftMerged = false;
		if (leftIter != _freeMemoryMap.end())
		{
			auto& leftHandle = leftIter->second;
			if (leftHandle.compressedOffset + leftHandle.compressedSize == descriptorMemoryHandle.compressedOffset)
			{
				isLeftMerged = true;

				leftHandle.compressedSize += descriptorMemoryHandle.compressedSize;
			}
		}
		
		auto& rightHandle = rightIter->second;
		if (descriptorMemoryHandle.compressedOffset + descriptorMemoryHandle.compressedSize == rightHandle.compressedOffset)
		{
			if (isLeftMerged)
			{
				auto& leftHandle = leftIter->second;
				leftHandle.compressedSize += rightHandle.compressedSize;
			}
			else
			{
				DescriptorMemoryHandle newHandle(descriptorMemoryHandle.compressedOffset, descriptorMemoryHandle.compressedSize + rightHandle.compressedSize);
				_freeMemoryMap.emplace(newHandle.compressedOffset, newHandle);
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
			if (leftHandle.compressedOffset + leftHandle.compressedSize == descriptorMemoryHandle.compressedOffset)
			{
				leftHandle.compressedSize += descriptorMemoryHandle.compressedSize;

				return;
			}
		}
	}

	_freeMemoryMap.emplace(descriptorMemoryHandle.compressedOffset, descriptorMemoryHandle);
}

void AirEngine::Runtime::Graphic::Manager::DescriptorManager::WriteToHostDescriptorMemory(DescriptorMemoryHandle descriptorMemoryHandle, uint8_t* dataPtr, uint32_t offset, uint32_t size)
{
	if ((descriptorMemoryHandle.compressedOffset + descriptorMemoryHandle.compressedSize) << _descriptorMemoryAlignmentStride > _currentSize || descriptorMemoryHandle.compressedSize == 0)
	{
		qFatal("Memory handle is not valid.");
	}

	size_t blockOffset = FromCompressed(descriptorMemoryHandle.compressedOffset);
	size_t blockSize = FromCompressed(descriptorMemoryHandle.compressedSize);
	
	if (size == 0)
	{
		qFatal("Can not write zero size data.");
	}

	if (offset + size >= blockSize)
	{
		qFatal("Data size is bigger than block.");
	}

	std::memcpy(_hostMemory.data() + blockOffset + offset, dataPtr, size);
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
	_hostMemory.resize(_currentSize, 0);
	DescriptorMemoryHandle newHandle(0, ToCompressed(_currentSize));
	_freeMemoryMap.emplace(newHandle.compressedOffset, newHandle);
}