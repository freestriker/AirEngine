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
	, Rendering::MaterialBindableAssetBase()
{

	vk::DescriptorAddressInfoEXT descriptorAddressInfo{};
	descriptorAddressInfo.address = BufferDeviceAddress();
	descriptorAddressInfo.range = Size();
	descriptorAddressInfo.format = vk::Format::eUndefined;

	vk::DescriptorGetInfoEXT descriptorGetInfo{};
	descriptorGetInfo.type = vk::DescriptorType::eUniformBuffer;
	descriptorGetInfo.data.pUniformBuffer = &descriptorAddressInfo;

	auto&& descriptorSize = Manager::DescriptorManager::DescriptorSize(vk::DescriptorType::eUniformBuffer);
	RawDescriptor().resize(descriptorSize);

	Graphic::Manager::DeviceManager::Device().getDescriptorEXT(
		&descriptorGetInfo,
		descriptorSize,
		RawDescriptor().data()
	);
}

AirEngine::Runtime::Graphic::Instance::UniformBuffer::~UniformBuffer()
{
}
