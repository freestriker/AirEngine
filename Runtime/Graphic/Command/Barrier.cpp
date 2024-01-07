#include "Barrier.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"
#include "AirEngine/Runtime/Graphic/Instance/Image.hpp"
#include "AirEngine/Runtime/Graphic/Instance/Buffer.hpp"

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

void AirEngine::Runtime::Graphic::Command::Barrier::AddBufferMemoryBarrier(const Instance::Buffer& buffer, vk::PipelineStageFlags2 srcStageMask, vk::AccessFlags2 srcAccessMask, vk::PipelineStageFlags2 dstStageMask, vk::AccessFlags2 dstAccessMask)
{
	vk::BufferMemoryBarrier2 bufferMemoryBarrier{};
	bufferMemoryBarrier.srcStageMask = srcStageMask;
	bufferMemoryBarrier.srcAccessMask = srcAccessMask;
	bufferMemoryBarrier.dstStageMask = dstStageMask;
	bufferMemoryBarrier.dstAccessMask = dstAccessMask;
	bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferMemoryBarrier.buffer = buffer.VkHandle();
	bufferMemoryBarrier.offset = buffer.Offset();
	bufferMemoryBarrier.size = buffer.Size();

	_bufferMemoryBarriers.emplace_back(std::move(bufferMemoryBarrier));
}
