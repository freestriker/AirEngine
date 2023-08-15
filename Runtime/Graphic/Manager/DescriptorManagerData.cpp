#include "DescriptorManagerData.hpp"
#include "DescriptorManager.hpp"

size_t AirEngine::Runtime::Graphic::Manager::DescriptorMemoryHandle::Offset() const
{
	return AirEngine::Runtime::Graphic::Manager::DescriptorManager::FromCompressed(offset);
}

size_t AirEngine::Runtime::Graphic::Manager::DescriptorMemoryHandle::Size() const
{
	return AirEngine::Runtime::Graphic::Manager::DescriptorManager::FromCompressed(size);
}
