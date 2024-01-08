#include "Image.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"

AirEngine::Runtime::Graphic::Instance::Image::~Image()
{
	if(!_isNative) Graphic::Manager::DeviceManager::Device().destroyImage(_image);
}

void AirEngine::Runtime::Graphic::Instance::Image::SetMemory(std::shared_ptr<Instance::Memory> memory)
{
	_memory = std::move(memory);
	auto bindResult = vmaBindImageMemory(Graphic::Manager::DeviceManager::VmaAllocator(), _memory->Allocation(), _image);
	if (VK_SUCCESS != bindResult) qFatal("Failed to bind image.");
}

void AirEngine::Runtime::Graphic::Instance::Image::AddView(Utility::InternedString name, vk::ImageViewType type, vk::ImageAspectFlags aspectMask, uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseArrayLayer, uint32_t layerCount)
{
	View newView{};
	newView.type = type;
	newView.subresource = vk::ImageSubresourceRange(aspectMask, baseMipLevel, levelCount, baseArrayLayer, layerCount);

	vk::ImageViewCreateInfo info({}, _image, type, _format, {}, newView.subresource);
	newView.imageView = Manager::DeviceManager::Device().createImageView(info);

	if(!_views.try_emplace(name, std::move(newView)).second) qFatal("Image failed to add new view.");
}

void AirEngine::Runtime::Graphic::Instance::Image::RemoveView(Utility::InternedString name)
{
	auto&& iter = _views.find(name);
	if (iter == _views.end()) qFatal("Image do not contain this view.");

	Manager::DeviceManager::Device().destroyImageView(iter->second.imageView);
	_views.erase(iter);
}

const std::unordered_map<AirEngine::Runtime::Utility::InternedString, AirEngine::Runtime::Graphic::Instance::Image::View>& AirEngine::Runtime::Graphic::Instance::Image::Views() const
{
	return _views;
}

AirEngine::Runtime::Graphic::Instance::Image::Image()
	: _format()
	, _extent3D()
	, _imageType()
	, _imageTiling()
	, _imageUsageFlags()
	, _imageCreateFlags()
	, _layerCount()
	, _mipmapLevelCount()
	, _image()
	, _memory()
	, _isNative(false)
{
}

AirEngine::Runtime::Graphic::Instance::Image* AirEngine::Runtime::Graphic::Instance::Image::PopulateDataAndCreateInstance(
	vk::Format format, 
	vk::Extent3D extent3D, 
	vk::ImageType imageType, 
	uint32_t layerCount, 
	uint32_t mipmapLevelCount,
	vk::ImageUsageFlags imageUsageFlags, 
	vk::MemoryPropertyFlags property, 
	vk::ImageTiling imageTiling, 
	vk::ImageCreateFlags imageCreateFlags, 
	VmaAllocationCreateFlags flags, 
	VmaMemoryUsage memoryUsage
)
{
	if (_image || _memory) qFatal("Already exist vulkan instance in this image.");

	_format = format;
	_extent3D = extent3D;
	_imageType = imageType;
	_imageTiling = imageTiling;
	_imageUsageFlags = imageUsageFlags;
	_imageCreateFlags = imageCreateFlags;
	_layerCount = layerCount;
	_mipmapLevelCount = mipmapLevelCount;
	_isNative = false;

	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VkImageType(_imageType);
	imageCreateInfo.extent = _extent3D;
	imageCreateInfo.mipLevels = _mipmapLevelCount;
	imageCreateInfo.arrayLayers = _layerCount;
	imageCreateInfo.format = VkFormat(_format);
	imageCreateInfo.tiling = VkImageTiling(_imageTiling);
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = VkImageUsageFlags(_imageUsageFlags);
	imageCreateInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.flags = VkImageCreateFlags(_imageCreateFlags);

	VmaAllocationCreateInfo vmaCreateInfo{};
	vmaCreateInfo.flags = flags;
	vmaCreateInfo.usage = memoryUsage;
	vmaCreateInfo.requiredFlags = property.operator unsigned int();

	VmaAllocationInfo vmaInfo{};
	VmaAllocation vmaAllocation{};
	VkImage vkImage{};
	auto result = vmaCreateImage(Graphic::Manager::DeviceManager::VmaAllocator(), &imageCreateInfo, &vmaCreateInfo, &vkImage, &vmaAllocation, &vmaInfo);

	if (VK_SUCCESS != result) qFatal("Failed to create image.");

	_image = vkImage;
	_memory = std::shared_ptr<Instance::Memory>(new Instance::Memory(vmaAllocation, vmaInfo));

	return this;
}

AirEngine::Runtime::Graphic::Instance::Image::Image(
	vk::Format format,
	vk::Extent3D extent3D,
	vk::ImageType imageType,
	uint32_t layerCount,
	uint32_t mipmapLevelCount,
	vk::ImageUsageFlags imageUsageFlags,
	vk::MemoryPropertyFlags property,
	vk::ImageTiling imageTiling,
	vk::ImageCreateFlags imageCreateFlags,
	VmaAllocationCreateFlags flags, VmaMemoryUsage memoryUsage
)
	: _format(format)
	, _extent3D(extent3D)
	, _imageType(imageType)
	, _imageTiling(imageTiling)
	, _imageUsageFlags(imageUsageFlags)
	, _imageCreateFlags(imageCreateFlags)
	, _layerCount(layerCount)
	, _mipmapLevelCount(mipmapLevelCount)
	, _image()
	, _memory()
	, _isNative(false)
{
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VkImageType(_imageType);
	imageCreateInfo.extent = _extent3D;
	imageCreateInfo.mipLevels = _mipmapLevelCount;
	imageCreateInfo.arrayLayers = _layerCount;
	imageCreateInfo.format = VkFormat(_format);
	imageCreateInfo.tiling = VkImageTiling(_imageTiling);
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = VkImageUsageFlags(_imageUsageFlags);
	imageCreateInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.flags = VkImageCreateFlags(_imageCreateFlags);

	VmaAllocationCreateInfo vmaCreateInfo{};
	vmaCreateInfo.flags = flags;
	vmaCreateInfo.usage = memoryUsage;
	vmaCreateInfo.requiredFlags = property.operator unsigned int();

	VmaAllocationInfo vmaInfo{};
	VmaAllocation vmaAllocation{};
	VkImage vkImage{};
	auto result = vmaCreateImage(Graphic::Manager::DeviceManager::VmaAllocator(), &imageCreateInfo, &vmaCreateInfo, &vkImage, &vmaAllocation, &vmaInfo);

	if (VK_SUCCESS != result) qFatal("Failed to create image.");

	_image = vkImage;
	_memory = std::shared_ptr<Instance::Memory>(new Instance::Memory(vmaAllocation, vmaInfo));
}

AirEngine::Runtime::Graphic::Instance::Image::Image(
	vk::Format format,
	vk::Extent3D extent3D,
	vk::ImageType imageType,
	uint32_t layerCount,
	uint32_t mipmapLevelCount,
	vk::ImageUsageFlags imageUsageFlags,
	std::shared_ptr<Instance::Memory> memory,
	vk::ImageTiling imageTiling,
	vk::ImageCreateFlags imageCreateFlags
)
	: _format(format)
	, _extent3D(extent3D)
	, _imageType(imageType)
	, _imageTiling(imageTiling)
	, _imageUsageFlags(imageUsageFlags)
	, _imageCreateFlags(imageCreateFlags)
	, _layerCount(layerCount)
	, _mipmapLevelCount(mipmapLevelCount)
	, _image()
	, _memory(memory)
	, _isNative(false)
{
	vk::ImageCreateInfo imageCreateInfo{};
	imageCreateInfo.imageType = _imageType;
	imageCreateInfo.extent = _extent3D;
	imageCreateInfo.mipLevels = _mipmapLevelCount;
	imageCreateInfo.arrayLayers = _layerCount;
	imageCreateInfo.format = _format;
	imageCreateInfo.tiling = _imageTiling;
	imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
	imageCreateInfo.usage = _imageUsageFlags;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
	imageCreateInfo.flags = _imageCreateFlags;

	_image = Graphic::Manager::DeviceManager::Device().createImage(imageCreateInfo);

	auto bindResult = vmaBindImageMemory(Graphic::Manager::DeviceManager::VmaAllocator(), _memory->Allocation(), _image);

	if (VkResult::VK_SUCCESS != bindResult) qFatal("Failed to bind image.");
}

AirEngine::Runtime::Graphic::Instance::Image::Image(
	vk::Format format,
	vk::Extent3D extent3D,
	vk::ImageType imageType,
	uint32_t layerCount,
	uint32_t mipmapLevelCount,
	vk::ImageUsageFlags imageUsageFlags,
	vk::ImageTiling imageTiling,
	vk::ImageCreateFlags imageCreateFlags
)
	: _format(format)
	, _extent3D(extent3D)
	, _imageType(imageType)
	, _imageTiling(imageTiling)
	, _imageUsageFlags(imageUsageFlags)
	, _imageCreateFlags(imageCreateFlags)
	, _layerCount(layerCount)
	, _mipmapLevelCount(mipmapLevelCount)
	, _image()
	, _memory()
	, _isNative(false)
{
	vk::ImageCreateInfo imageCreateInfo{};
	imageCreateInfo.imageType = _imageType;
	imageCreateInfo.extent = _extent3D;
	imageCreateInfo.mipLevels = _mipmapLevelCount;
	imageCreateInfo.arrayLayers = _layerCount;
	imageCreateInfo.format = _format;
	imageCreateInfo.tiling = _imageTiling;
	imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
	imageCreateInfo.usage = _imageUsageFlags;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
	imageCreateInfo.flags = _imageCreateFlags;

	_image = Graphic::Manager::DeviceManager::Device().createImage(imageCreateInfo);
}

AirEngine::Runtime::Graphic::Instance::Image::Image(
	vk::Image image,
	vk::Format format,
	vk::Extent3D extent3D,
	vk::ImageType imageType,
	uint32_t layerCount,
	uint32_t mipmapLevelCount,
	vk::ImageUsageFlags imageUsageFlags,
	vk::ImageTiling imageTiling,
	vk::ImageCreateFlags imageCreateFlags
)
	: _format(format)
	, _extent3D(extent3D)
	, _imageType(imageType)
	, _imageTiling(imageTiling)
	, _imageUsageFlags(imageUsageFlags)
	, _imageCreateFlags(imageCreateFlags)
	, _layerCount(layerCount)
	, _mipmapLevelCount(mipmapLevelCount)
	, _image(image)
	, _memory()
	, _isNative(true)
{
}

AirEngine::Runtime::Graphic::Instance::Image* AirEngine::Runtime::Graphic::Instance::Image::CreateSwapchainImage(
	vk::Image image, 
	vk::Format format, 
	vk::Extent2D extent2D, 
	vk::ImageUsageFlags imageUsageFlags
)
{
	return new Image(image, format, { extent2D.width, extent2D.height, 1 }, vk::ImageType::e2D, 1, 1, imageUsageFlags);
}
