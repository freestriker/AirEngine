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
	vk::PipelineStageFlags2 srcStageMask, 
	vk::AccessFlags2 srcAccessMask, 
	vk::PipelineStageFlags2 dstStageMask, 
	vk::AccessFlags2 dstAccessMask, 
	vk::ImageLayout oldLayout, 
	vk::ImageLayout newLayout, 
	vk::ImageAspectFlags aspectMask
)
{
	vk::ImageMemoryBarrier2 imBarrier{};
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
