#include "Image.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"

AirEngine::Runtime::Graphic::Instance::Image::~Image()
{
	if(!_isNative) vkDestroyImage(Core::Manager::GraphicDeviceManager::VkDevice(), _image, nullptr);
}

void AirEngine::Runtime::Graphic::Instance::Image::SetMemory(std::shared_ptr<Instance::Memory> memory)
{
	_memory = std::move(memory);
	auto bindResult = vmaBindImageMemory(Core::Manager::GraphicDeviceManager::VmaAllocator(), _memory->Allocation(), _image);
	if (VK_SUCCESS != bindResult) qFatal("Failed to bind image.");
}

AirEngine::Runtime::Graphic::Instance::Image::Image(
	VkFormat format,
	VkExtent3D extent3D,
	VkImageType imageType,
	uint32_t layerCount,
	uint32_t mipmapLevelCount,
	VkImageUsageFlags imageUsageFlags,
	VkMemoryPropertyFlags property,
	VkImageTiling imageTiling,
	VkImageCreateFlags imageCreateFlags,
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
	, _image(VK_NULL_HANDLE)
	, _memory()
	, _isNative(false)
{
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = _imageType;
	imageCreateInfo.extent = _extent3D;
	imageCreateInfo.mipLevels = _mipmapLevelCount;
	imageCreateInfo.arrayLayers = _layerCount;
	imageCreateInfo.format = _format;
	imageCreateInfo.tiling = _imageTiling;
	imageCreateInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = _imageUsageFlags;
	imageCreateInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.flags = _imageCreateFlags;

	VmaAllocationCreateInfo vmaCreateInfo{};
	vmaCreateInfo.flags = flags;
	vmaCreateInfo.usage = memoryUsage;
	vmaCreateInfo.requiredFlags = property;

	VmaAllocationInfo vmaInfo{};
	VmaAllocation vmaAllocation;
	auto result = vmaCreateImage(Core::Manager::GraphicDeviceManager::VmaAllocator(), &imageCreateInfo, &vmaCreateInfo, &_image, &vmaAllocation, &vmaInfo);

	if (VK_SUCCESS != result) qFatal("Failed to create image.");

	_memory = std::shared_ptr<Instance::Memory>(new Instance::Memory(vmaAllocation, vmaInfo));
}

AirEngine::Runtime::Graphic::Instance::Image::Image(
	VkFormat format,
	VkExtent3D extent3D,
	VkImageType imageType,
	uint32_t layerCount,
	uint32_t mipmapLevelCount,
	VkImageUsageFlags imageUsageFlags,
	std::shared_ptr<Instance::Memory> memory,
	VkImageTiling imageTiling,
	VkImageCreateFlags imageCreateFlags
)
	: _format(format)
	, _extent3D(extent3D)
	, _imageType(imageType)
	, _imageTiling(imageTiling)
	, _imageUsageFlags(imageUsageFlags)
	, _imageCreateFlags(imageCreateFlags)
	, _layerCount(layerCount)
	, _mipmapLevelCount(mipmapLevelCount)
	, _image(VK_NULL_HANDLE)
	, _memory()
	, _isNative(false)
{
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = _imageType;
	imageCreateInfo.extent = _extent3D;
	imageCreateInfo.mipLevels = _mipmapLevelCount;
	imageCreateInfo.arrayLayers = _layerCount;
	imageCreateInfo.format = _format;
	imageCreateInfo.tiling = _imageTiling;
	imageCreateInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = _imageUsageFlags;
	imageCreateInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.flags = _imageCreateFlags;

	auto imageResult = vkCreateImage(Core::Manager::GraphicDeviceManager::VkDevice(), &imageCreateInfo, nullptr, &_image);

	if (VK_SUCCESS != imageResult) qFatal("Failed to create image.");

	auto bindResult = vmaBindImageMemory(Core::Manager::GraphicDeviceManager::VmaAllocator(), _memory->Allocation(), _image);

	if (VK_SUCCESS != bindResult) qFatal("Failed to bind image.");
}

AirEngine::Runtime::Graphic::Instance::Image::Image(
	VkFormat format,
	VkExtent3D extent3D,
	VkImageType imageType,
	uint32_t layerCount,
	uint32_t mipmapLevelCount,
	VkImageUsageFlags imageUsageFlags,
	VkImageTiling imageTiling,
	VkImageCreateFlags imageCreateFlags
)
	: _format(format)
	, _extent3D(extent3D)
	, _imageType(imageType)
	, _imageTiling(imageTiling)
	, _imageUsageFlags(imageUsageFlags)
	, _imageCreateFlags(imageCreateFlags)
	, _layerCount(layerCount)
	, _mipmapLevelCount(mipmapLevelCount)
	, _image(VK_NULL_HANDLE)
	, _memory()
	, _isNative(false)
{
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = _imageType;
	imageCreateInfo.extent = _extent3D;
	imageCreateInfo.mipLevels = _mipmapLevelCount;
	imageCreateInfo.arrayLayers = _layerCount;
	imageCreateInfo.format = _format;
	imageCreateInfo.tiling = _imageTiling;
	imageCreateInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = _imageUsageFlags;
	imageCreateInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.flags = _imageCreateFlags;

	auto imageResult = vkCreateImage(Core::Manager::GraphicDeviceManager::VkDevice(), &imageCreateInfo, nullptr, &_image);

	if (VK_SUCCESS != imageResult) qFatal("Failed to create image.");
}

AirEngine::Runtime::Graphic::Instance::Image::Image(
	VkImage image,
	VkFormat format,
	VkExtent3D extent3D,
	VkImageType imageType,
	uint32_t layerCount,
	uint32_t mipmapLevelCount,
	VkImageUsageFlags imageUsageFlags,
	VkImageTiling imageTiling,
	VkImageCreateFlags imageCreateFlags
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
	VkImage image, 
	VkFormat format, 
	VkExtent2D extent2D, 
	VkImageUsageFlags imageUsageFlags
)
{
	return new Image(image, format, { extent2D.width, extent2D.height, 1 }, VK_IMAGE_TYPE_2D, 1, 1, imageUsageFlags);
}
