#include "UniformBuffer.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DescriptorManager.hpp"

AirEngine::Runtime::Graphic::Instance::UniformBuffer::UniformBuffer(
	vk::DeviceSize size, 
	vk::BufferUsageFlags bufferUsage,
	vk::MemoryPropertyFlags property, 
	VmaAllocationCreateFlags flags, 
	VmaMemoryUsage memoryUsage
)
	: Buffer(size, bufferUsage | vk::BufferUsageFlagBits::eUniformBuffer, property, flags, memoryUsage)
	, _descriptorData(Manager::DescriptorManager::DescriptorSize(vk::DescriptorType::eUniformBuffer))
{
	vk::DescriptorAddressInfoEXT descriptorAddressInfo{};
	descriptorAddressInfo.address = BufferDeviceAddress();
	descriptorAddressInfo.range = Size();
	descriptorAddressInfo.format = vk::Format::eUndefined;

	vk::DescriptorGetInfoEXT descriptorGetInfo{};
	descriptorGetInfo.type = vk::DescriptorType::eUniformBuffer;
	descriptorGetInfo.data.pUniformBuffer = &descriptorAddressInfo;

	auto&& descriptorSize = Manager::DescriptorManager::DescriptorSize(vk::DescriptorType::eUniformBuffer);

	Graphic::Manager::DeviceManager::Device().getDescriptorEXT(
		&descriptorGetInfo,
		descriptorSize,
		_descriptorData.data()
	);
}

AirEngine::Runtime::Graphic::Instance::UniformBuffer::~UniformBuffer()
{
}

void AirEngine::Runtime::Graphic::Instance::UniformBuffer::SetDescriptorData(uint8_t* targetPtr, vk::DescriptorType descriptorType)
{
	if (descriptorType != vk::DescriptorType::eUniformBuffer) qFatal("Can not write to defferent descriptor type's memory.");

	std::memcpy(targetPtr, _descriptorData.data(), _descriptorData.size());
}
