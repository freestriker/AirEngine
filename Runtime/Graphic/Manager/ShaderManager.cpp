#include "ShaderManager.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"

std::unordered_map<vk::DescriptorType, uint8_t> AirEngine::Runtime::Graphic::Manager::ShaderManager::_descriptorTypeToSizeMap{};
uint8_t AirEngine::Runtime::Graphic::Manager::ShaderManager::_offsetAlignment{};

void AirEngine::Runtime::Graphic::Manager::ShaderManager::Initialize()
{
	auto&& physicalDeviceDescriptorBufferPropertiesEXT = vk::PhysicalDeviceDescriptorBufferPropertiesEXT();

	auto&& physicalDeviceProperties = vk::PhysicalDeviceProperties2();
	physicalDeviceProperties.pNext = &physicalDeviceDescriptorBufferPropertiesEXT;

	Core::Manager::GraphicDeviceManager::PhysicalDevice().getProperties2(&physicalDeviceProperties);

	_offsetAlignment = physicalDeviceDescriptorBufferPropertiesEXT.descriptorBufferOffsetAlignment;

	_descriptorTypeToSizeMap[vk::DescriptorType::eUniformBuffer] =physicalDeviceDescriptorBufferPropertiesEXT.uniformBufferDescriptorSize;
	_descriptorTypeToSizeMap[vk::DescriptorType::eCombinedImageSampler] =physicalDeviceDescriptorBufferPropertiesEXT.combinedImageSamplerDescriptorSize;
	_descriptorTypeToSizeMap[vk::DescriptorType::eStorageBuffer] =physicalDeviceDescriptorBufferPropertiesEXT.storageBufferDescriptorSize;
	_descriptorTypeToSizeMap[vk::DescriptorType::eStorageImage] =physicalDeviceDescriptorBufferPropertiesEXT.storageImageDescriptorSize;
	_descriptorTypeToSizeMap[vk::DescriptorType::eInputAttachment] =physicalDeviceDescriptorBufferPropertiesEXT.inputAttachmentDescriptorSize;
}
