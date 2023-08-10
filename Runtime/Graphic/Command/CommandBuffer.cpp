#include "CommandBuffer.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"
#include "CommandPool.hpp"
#include "Barrier.hpp"
#include "../Instance/Image.hpp"
#include "../Instance/Buffer.hpp"
#include <vulkan/vulkan_format_traits.hpp>

AirEngine::Runtime::Graphic::Command::CommandBuffer::CommandBuffer(Utility::InternedString commandBufferName, Command::CommandPool* commandPool, vk::CommandBufferLevel level)
	: _name(commandBufferName)
	, _vkCommandBuffer()
	, _vkCommandBufferLevel(level)
	, _commandPool(commandPool)
{
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = commandPool->VkHandle();
    allocInfo.level = _vkCommandBufferLevel;
    allocInfo.commandBufferCount = 1;

    _vkCommandBuffer = Core::Manager::GraphicDeviceManager::Device().allocateCommandBuffers(allocInfo).at(0);
}

AirEngine::Runtime::Graphic::Command::CommandBuffer::~CommandBuffer()
{
    Core::Manager::GraphicDeviceManager::Device().freeCommandBuffers(_commandPool->VkHandle(), _vkCommandBuffer);
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::Reset()
{
    if (_commandPool->VkCreateFlags() & vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
    {
        _vkCommandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    }
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::BeginRecord(vk::CommandBufferUsageFlags flags)
{
    vk::CommandBufferBeginInfo info{};
    info.flags = flags;

    _vkCommandBuffer.begin(info);
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::EndRecord()
{
    _vkCommandBuffer.end();
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::AddPipelineBarrier(const Barrier& barrier, vk::DependencyFlags dependencyFlags)
{
    vk::DependencyInfo info{};
    info.dependencyFlags = dependencyFlags;
    info.memoryBarrierCount = uint32_t(barrier._memoryBarriers.size());
    info.pMemoryBarriers = barrier._memoryBarriers.data();
    info.bufferMemoryBarrierCount = uint32_t(barrier._bufferMemoryBarriers.size());
    info.pBufferMemoryBarriers = barrier._bufferMemoryBarriers.data();
    info.imageMemoryBarrierCount = uint32_t(barrier._imageMemoryBarriers.size());
    info.pImageMemoryBarriers = barrier._imageMemoryBarriers.data();

    _vkCommandBuffer.pipelineBarrier2(info);
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::ClearColorImage(const Instance::Image& image, vk::ImageLayout imageLayout, const vk::ClearColorValue& color)
{
    vk::ImageSubresourceRange range{};
    range.baseMipLevel = 0;
    range.levelCount = image.MipmapLevelCount();
    range.baseArrayLayer = 0;
    range.layerCount = image.LayerCount();
    
    _vkCommandBuffer.clearColorImage(image.VkHandle(), imageLayout, color, range);
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::CopyBufferToImage(const Instance::Buffer& buffer, const Instance::Image& image, vk::ImageLayout imageLayout, vk::ImageAspectFlags imageAspectFlags)
{
    auto&& pmlExtent = image.PerMipmapLevelExtent3D();
    auto blockByteSize = vk::blockSize(vk::Format(image.Format()));
    VkDeviceSize offset = 0;

    std::vector< vk::BufferImageCopy> copys(pmlExtent.size(), VkBufferImageCopy{});
    for (uint32_t mipmapLevelIndex = 0; mipmapLevelIndex < copys.size(); mipmapLevelIndex++)
    {
        vk::BufferImageCopy& copy = copys[mipmapLevelIndex];

        copy.bufferOffset = offset;
        copy.bufferRowLength = 0;
        copy.bufferImageHeight = 0;
        copy.imageSubresource.aspectMask = imageAspectFlags;
        copy.imageSubresource.baseArrayLayer = 0;
        copy.imageSubresource.layerCount = image.LayerCount();
        copy.imageSubresource.mipLevel = mipmapLevelIndex;
        copy.imageOffset = vk::Offset3D{ 0, 0, 0 };
        copy.imageExtent = pmlExtent[mipmapLevelIndex];

        offset += blockByteSize * pmlExtent[mipmapLevelIndex].width * pmlExtent[mipmapLevelIndex].height * pmlExtent[mipmapLevelIndex].depth;
    }

    _vkCommandBuffer.copyBufferToImage(buffer.VkHandle(), image.VkHandle(), imageLayout, copys);
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::CopyBuffer(const Instance::Buffer& srcBuffer, const Instance::Buffer& dstBuffer, const std::vector<std::tuple<vk::DeviceSize, vk::DeviceSize, vk::DeviceSize>> srcOffsetDstOffsetSizes)
{
    std::vector<vk::BufferCopy> copys(srcOffsetDstOffsetSizes.size(), vk::BufferCopy{});
    for (uint32_t regionIndex = 0; regionIndex < copys.size(); regionIndex++)
    {
        vk::BufferCopy& copy = copys[regionIndex];
        copy.srcOffset = std::get<0>(srcOffsetDstOffsetSizes[regionIndex]);
        copy.dstOffset = std::get<1>(srcOffsetDstOffsetSizes[regionIndex]);
        copy.size = std::get<2>(srcOffsetDstOffsetSizes[regionIndex]);
    }
    _vkCommandBuffer.copyBuffer(srcBuffer.VkHandle(), dstBuffer.VkHandle(), copys);
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::Blit(const Instance::Image& srcImage, vk::ImageLayout srcImageLayout, const Instance::Image& dstImage, vk::ImageLayout dstImageLayout, vk::ImageAspectFlags imageAspectFlags, vk::Filter filter)
{
    auto&& layerCount = std::min(srcImage.LayerCount(), dstImage.LayerCount());
    auto&& mipmapLevelCount = std::min(srcImage.MipmapLevelCount(), dstImage.MipmapLevelCount());
    auto&& srcPmlExtent = srcImage.PerMipmapLevelExtent3D();
    auto&& dstPmlExtent = dstImage.PerMipmapLevelExtent3D();

    std::vector<vk::ImageBlit> blits(mipmapLevelCount);
    for (uint32_t i = 0; i < mipmapLevelCount; i++)
    {
        auto& blit = blits[i];
        auto&& srcExtent = srcPmlExtent[i];
        auto&& dstExtent = dstPmlExtent[i];
        blit.srcSubresource.aspectMask = imageAspectFlags;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = layerCount;
        blit.srcSubresource.mipLevel = i;
        blit.srcOffsets[0] = vk::Offset3D{ 0, 0, 0 };
        blit.srcOffsets[1] = vk::Offset3D{ static_cast<int>(srcExtent.width), static_cast<int>(srcExtent.height), static_cast<int>(srcExtent.depth) };
        blit.dstSubresource.aspectMask = imageAspectFlags;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = layerCount;
        blit.dstSubresource.mipLevel = i;
        blit.dstOffsets[0] = vk::Offset3D{ 0, 0, 0 };
        blit.dstOffsets[1] = vk::Offset3D{ static_cast<int>(dstExtent.width), static_cast<int>(dstExtent.height), static_cast<int>(dstExtent.depth) };
    }

    _vkCommandBuffer.blitImage(srcImage.VkHandle(), srcImageLayout, dstImage.VkHandle(), dstImageLayout, blits, filter);
}
