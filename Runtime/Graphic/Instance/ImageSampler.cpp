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
	CreateVulkanInstance();
}

AirEngine::Runtime::Graphic::Instance::ImageSampler::ImageSampler(vk::Filter filter, vk::SamplerMipmapMode mipmapMode, vk::SamplerAddressMode addressMode, float mipmapLevel)
	: AirEngine::Runtime::Graphic::Rendering::MaterialBindableAssetBase()
	, _magFilter(filter)
	, _minFilter(filter)
	, _mipmapMode(mipmapMode)
	, _addressModeU(addressMode)
	, _addressModeV(addressMode)
	, _addressModeW(addressMode)
	, _minMipmapLevel(mipmapLevel)
	, _maxMipmapLevel(mipmapLevel)
	, _sampler()
{
	CreateVulkanInstance();
}

AirEngine::Runtime::Graphic::Instance::ImageSampler::ImageSampler(vk::Filter filter, vk::SamplerMipmapMode mipmapMode, vk::SamplerAddressMode addressMode, float minMipmapLevel, float maxMipmapLevel)
	: AirEngine::Runtime::Graphic::Rendering::MaterialBindableAssetBase()
	, _magFilter(filter)
	, _minFilter(filter)
	, _mipmapMode(mipmapMode)
	, _addressModeU(addressMode)
	, _addressModeV(addressMode)
	, _addressModeW(addressMode)
	, _minMipmapLevel(minMipmapLevel)
	, _maxMipmapLevel(maxMipmapLevel)
	, _sampler()
{
	CreateVulkanInstance();
}

AirEngine::Runtime::Graphic::Instance::ImageSampler::~ImageSampler()
{
	Manager::DeviceManager::Device().destroySampler(_sampler);
}

void AirEngine::Runtime::Graphic::Instance::ImageSampler::CreateVulkanInstance()
{
	vk::SamplerCreateInfo info({}, _magFilter, _minFilter, _mipmapMode, _addressModeU, _addressModeV, _addressModeW, 0, vk::Bool32(false), 0, vk::Bool32(false), VULKAN_HPP_NAMESPACE::CompareOp::eNever, _minMipmapLevel, _maxMipmapLevel);
	_sampler = Manager::DeviceManager::Device().createSampler(info);
}

void AirEngine::Runtime::Graphic::Instance::ImageSampler::SetDescriptorData(uint8_t* targetPtr, vk::DescriptorType descriptorType)
{
	vk::DescriptorGetInfoEXT descriptorGetInfo{ vk::DescriptorType::eSampler };
	descriptorGetInfo.data.pSampler = &_sampler;

	Manager::DeviceManager::Device().getDescriptorEXT(&descriptorGetInfo, Manager::DescriptorManager::DescriptorSize(vk::DescriptorType::eSampler), targetPtr);
}
