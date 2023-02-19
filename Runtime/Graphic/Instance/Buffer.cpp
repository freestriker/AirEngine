#include "Buffer.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"

AirEngine::Runtime::Graphic::Instance::Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags property, VmaAllocationCreateFlags flags, VmaMemoryUsage memoryUsage)
	: _vkBuffer(VK_NULL_HANDLE)
	, _size(size)
	, _usage(bufferUsage)
	, _memory()
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = bufferUsage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo vmaCreateInfo{};
	vmaCreateInfo.flags = flags;
	vmaCreateInfo.usage = memoryUsage;
	vmaCreateInfo.requiredFlags = property;

	VmaAllocationInfo vmaInfo{};
	VmaAllocation vmaAllocation;
	auto result = vmaCreateBuffer(Core::Manager::GraphicDeviceManager::VmaAllocator(), &bufferCreateInfo, &vmaCreateInfo, &_vkBuffer, &vmaAllocation, &vmaInfo);

	if (VK_SUCCESS != result) qFatal("Failed to create buffer.");

	_memory = std::shared_ptr<Instance::Memory>(new Instance::Memory(vmaAllocation, vmaInfo));
}

AirEngine::Runtime::Graphic::Instance::Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags bufferUsage, std::shared_ptr<Instance::Memory> memory)
	: _vkBuffer(VK_NULL_HANDLE)
	, _size(size)
	, _usage(bufferUsage)
	, _memory(memory)
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = bufferUsage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	auto bufferResult = vkCreateBuffer(Core::Manager::GraphicDeviceManager::VkDevice(), &bufferCreateInfo, nullptr, &_vkBuffer);

	if (VK_SUCCESS != bufferResult) qFatal("Failed to create buffer.");

	auto bindResult = vmaBindBufferMemory(Core::Manager::GraphicDeviceManager::VmaAllocator(), _memory->Allocation(), _vkBuffer);

	if (VK_SUCCESS != bindResult) qFatal("Failed to bind buffer.");
}

AirEngine::Runtime::Graphic::Instance::Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags bufferUsage)
	: _vkBuffer(VK_NULL_HANDLE)
	, _size(size)
	, _usage(bufferUsage)
	, _memory()
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = bufferUsage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	auto result = vkCreateBuffer(Core::Manager::GraphicDeviceManager::VkDevice(), &bufferCreateInfo, nullptr, &_vkBuffer);

	if (VK_SUCCESS != result) qFatal("Failed to create buffer.");
}

AirEngine::Runtime::Graphic::Instance::Buffer::~Buffer()
{
	vkDestroyBuffer(Core::Manager::GraphicDeviceManager::VkDevice(), _vkBuffer, nullptr);
}
