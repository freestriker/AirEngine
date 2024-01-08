#include "ImageSampler.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DescriptorManager.hpp"

AirEngine::Runtime::Graphic::Instance::ImageSampler::ImageSampler(
	vk::Filter magFilter, 
	vk::Filter minFilter,
	vk::SamplerMipmapMode mipmapMode, 
	vk::SamplerAddressMode addressModeU, 
	vk::SamplerAddressMode addressModeV, 
	vk::SamplerAddressMode addressModeW, 
	float minMipmapLevel, 
	float maxMipmapLevel
)
	: AirEngine::Runtime::Graphic::Rendering::MaterialBindableAssetBase()
	, _magFilter(magFilter)
	, _minFilter(minFilter)
	, _mipmapMode(mipmapMode)
	, _addressModeU(addressModeU)
	, _addressModeV(addressModeV)
	, _addressModeW(addressModeW)
	, _minMipmapLevel(minMipmapLevel)
	, _maxMipmapLevel(maxMipmapLevel)
	, _sampler()

{
	vk::SamplerCreateInfo info({}, magFilter, minFilter, mipmapMode, addressModeU, addressModeV, addressModeW, 0, vk::Bool32(false), 0, vk::Bool32(false), VULKAN_HPP_NAMESPACE::CompareOp::eNever, minMipmapLevel, maxMipmapLevel);
	_sampler = Manager::DeviceManager::Device().createSampler(info);

	vk::DescriptorGetInfoEXT descriptorGetInfo{};
	descriptorGetInfo.type = vk::DescriptorType::eSampler;
	descriptorGetInfo.data.pSampler = &_sampler;

	auto&& descriptorSize = Manager::DescriptorManager::DescriptorSize(vk::DescriptorType::eSampler);
	RawDescriptor().resize(descriptorSize);

	Graphic::Manager::DeviceManager::Device().getDescriptorEXT(
		&descriptorGetInfo,
		descriptorSize,
		RawDescriptor().data()
	);
}

AirEngine::Runtime::Graphic::Instance::ImageSampler::~ImageSampler()
{
	Manager::DeviceManager::Device().destroySampler(_sampler);
}
