#include "Memory.hpp"
#include "Buffer.hpp"

AirEngine::Runtime::Graphic::Instance::Memory::Memory(VmaAllocation vmaAllocation, const VmaAllocationInfo& vmaInfo)
	: _vmaAllocation(vmaAllocation)
	, _memoryType(vmaInfo.memoryType)
	, _vkDeviceMemory(vmaInfo.deviceMemory)
	, _offset(vmaInfo.offset)
	, _size(vmaInfo.size)
{

}

AirEngine::Runtime::Graphic::Instance::Memory::~Memory()
{
	vmaFreeMemory(Core::Manager::GraphicDeviceManager::VmaAllocator(), _vmaAllocation);
}
