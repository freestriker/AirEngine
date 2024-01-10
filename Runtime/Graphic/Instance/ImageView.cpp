#include "ImageView.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DescriptorManager.hpp"
#include "Image.hpp"

AirEngine::Runtime::Graphic::Instance::ImageView::ImageView(Instance::Image* image, Utility::InternedString name, vk::ImageViewType type, vk::ImageLayout layout, vk::ImageAspectFlags aspectMask, uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseArrayLayer, uint32_t layerCount)
	: Rendering::MaterialBindableAssetBase()
	, _name(name)
	, _vkImageView()
	, _vkImageLayout(layout)
	, _image(image)
	, _imageViewType(type)
	, _imageSubresourceRange(aspectMask, baseMipLevel, levelCount, baseArrayLayer, layerCount)
{
	vk::ImageViewCreateInfo imageViewCreateInfo({}, _image->VkHandle(), _imageViewType, _image->Format(), {}, _imageSubresourceRange);
	_vkImageView = Manager::DeviceManager::Device().createImageView(imageViewCreateInfo);
}

AirEngine::Runtime::Graphic::Instance::ImageView::~ImageView()
{
	Manager::DeviceManager::Device().destroyImageView(_vkImageView);
}

void AirEngine::Runtime::Graphic::Instance::ImageView::SetDescriptorData(uint8_t* targetPtr, vk::DescriptorType descriptorType)
{
	vk::DescriptorGetInfoEXT descriptorGetInfo{};
	vk::DescriptorImageInfo descriptorImageInfo({}, _vkImageView, _vkImageLayout);
	descriptorGetInfo.data.pSampledImage = &descriptorImageInfo;

	Manager::DeviceManager::Device().getDescriptorEXT(&descriptorGetInfo, Manager::DescriptorManager::DescriptorSize(descriptorType), targetPtr);
}
