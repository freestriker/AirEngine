#include "Barrier.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"
#include "../Instance/Image.hpp"

AirEngine::Runtime::Graphic::Command::Barrier::Barrier()
	: _memoryBarriers()
	, _bufferMemoryBarriers()
	, _imageMemoryBarriers()
{
}

void AirEngine::Runtime::Graphic::Command::Barrier::AddImageMemoryBarrier(
	const Instance::Image& image, 
	VkPipelineStageFlags2 srcStageMask, 
	VkAccessFlags2 srcAccessMask, 
	VkPipelineStageFlags2 dstStageMask, 
	VkAccessFlags2 dstAccessMask, 
	VkImageLayout oldLayout, 
	VkImageLayout newLayout, 
	VkImageAspectFlags aspectMask
)
{
	VkImageMemoryBarrier2 imBarrier{};
	imBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	imBarrier.pNext = nullptr;
	imBarrier.srcStageMask = srcStageMask;
	imBarrier.srcAccessMask = srcAccessMask;
	imBarrier.dstStageMask = dstStageMask;
	imBarrier.dstAccessMask = dstAccessMask;
	imBarrier.oldLayout = oldLayout;
	imBarrier.newLayout = newLayout;
	imBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imBarrier.image = image.VkHandle();
	imBarrier.subresourceRange.aspectMask = aspectMask;
	imBarrier.subresourceRange.baseMipLevel = 0;
	imBarrier.subresourceRange.levelCount = image.MipmapLevelCount();
	imBarrier.subresourceRange.baseArrayLayer = 0;
	imBarrier.subresourceRange.layerCount = image.LayerCount();

	_imageMemoryBarriers.emplace_back(imBarrier);
}
