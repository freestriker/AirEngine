#include "Buffer.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"

AirEngine::Runtime::Graphic::Instance::Buffer::Buffer(vk::DeviceSize size, vk::BufferUsageFlags bufferUsage, vk::MemoryPropertyFlags property, VmaAllocationCreateFlags flags, VmaMemoryUsage memoryUsage)
	: Rendering::MaterialBindableAssetBase()
	, _vkBuffer(VK_NULL_HANDLE)
	, _size(size)
	, _usage(bufferUsage)
	, _memory()
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = bufferUsage.operator unsigned int() | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo vmaCreateInfo{};
	vmaCreateInfo.flags = flags;
	vmaCreateInfo.usage = memoryUsage;
	vmaCreateInfo.requiredFlags = property.operator unsigned int();

	VmaAllocationInfo vmaInfo{};
	VmaAllocation vmaAllocation;
	VkBuffer vkBuffer{};
	auto result = vmaCreateBuffer(Graphic::Manager::DeviceManager::VmaAllocator(), &bufferCreateInfo, &vmaCreateInfo, &vkBuffer, &vmaAllocation, &vmaInfo);

	if (VK_SUCCESS != result) qFatal("Failed to create buffer.");

	_vkBuffer = vkBuffer;
	_memory = std::shared_ptr<Instance::Memory>(new Instance::Memory(vmaAllocation, vmaInfo));

	{
		vk::BufferDeviceAddressInfoKHR bufferDeviceAddressInfo{};
		bufferDeviceAddressInfo.buffer = _vkBuffer;

		_bufferAddress = Graphic::Manager::DeviceManager::Device().getBufferAddress(bufferDeviceAddressInfo);
	}
}

AirEngine::Runtime::Graphic::Instance::Buffer::Buffer(vk::DeviceSize size, vk::BufferUsageFlags bufferUsage, std::shared_ptr<Instance::Memory> memory)
	: Rendering::MaterialBindableAssetBase()
	, _vkBuffer(VK_NULL_HANDLE)
	, _size(size)
	, _usage(bufferUsage)
	, _memory(memory)
{
	vk::BufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = bufferUsage | vk::BufferUsageFlagBits::eShaderDeviceAddress;
	bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

	_vkBuffer = Graphic::Manager::DeviceManager::Device().createBuffer(bufferCreateInfo);

	auto bindResult = vmaBindBufferMemory(Graphic::Manager::DeviceManager::VmaAllocator(), _memory->Allocation(), _vkBuffer);

	if (VK_SUCCESS != bindResult) qFatal("Failed to bind buffer.");
	{
		vk::BufferDeviceAddressInfoKHR bufferDeviceAddressInfo{};
		bufferDeviceAddressInfo.buffer = _vkBuffer;

		_bufferAddress = Graphic::Manager::DeviceManager::Device().getBufferAddress(bufferDeviceAddressInfo);
	}
}

AirEngine::Runtime::Graphic::Instance::Buffer::Buffer(vk::DeviceSize size, vk::BufferUsageFlags bufferUsage)
	: Rendering::MaterialBindableAssetBase()
	, _vkBuffer(VK_NULL_HANDLE)
	, _size(size)
	, _usage(bufferUsage)
	, _memory()
{
	vk::BufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = bufferUsage | vk::BufferUsageFlagBits::eShaderDeviceAddress;
	bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

	_vkBuffer = Graphic::Manager::DeviceManager::Device().createBuffer(bufferCreateInfo);

	{
		vk::BufferDeviceAddressInfoKHR bufferDeviceAddressInfo{};
		bufferDeviceAddressInfo.buffer = _vkBuffer;

		_bufferAddress = Graphic::Manager::DeviceManager::Device().getBufferAddress(bufferDeviceAddressInfo);
	}
}

AirEngine::Runtime::Graphic::Instance::Buffer::~Buffer()
{
	Graphic::Manager::DeviceManager::Device().destroyBuffer(_vkBuffer);
}
