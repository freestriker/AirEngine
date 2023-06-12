#include "CommandBuffer.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"
#include "CommandPool.hpp"
#include "Barrier.hpp"
#include "../Instance/Image.hpp"
#include "../Instance/Buffer.hpp"
#include <vulkan/vulkan_format_traits.hpp>

AirEngine::Runtime::Graphic::Command::CommandBuffer::CommandBuffer(Utility::InternedString commandBufferName, Command::CommandPool* commandPool, VkCommandBufferLevel level)
	: _name(commandBufferName)
	, _vkCommandBuffer(VK_NULL_HANDLE)
	, _vkCommandBufferLevel(level)
	, _commandPool(commandPool)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool->VkHandle();
    allocInfo.level = _vkCommandBufferLevel;
    allocInfo.commandBufferCount = 1;

    auto result = vkAllocateCommandBuffers(Core::Manager::GraphicDeviceManager::VkDevice(), &allocInfo, &_vkCommandBuffer);
    if (result != VK_SUCCESS)
    {
        qFatal("Failed to allocate command buffer.");
    }
}

AirEngine::Runtime::Graphic::Command::CommandBuffer::~CommandBuffer()
{
    vkFreeCommandBuffers(Core::Manager::GraphicDeviceManager::VkDevice(), _commandPool->VkHandle(), 1, &_vkCommandBuffer);
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::Reset()
{
    if (_commandPool->VkCreateFlags() & VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
    {
        vkResetCommandBuffer(_vkCommandBuffer, VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    }
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::BeginRecord(VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    info.pInheritanceInfo = nullptr;

    auto&& result = vkBeginCommandBuffer(_vkCommandBuffer, &info);
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::EndRecord()
{
    vkEndCommandBuffer(_vkCommandBuffer);
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::AddPipelineBarrier(const Barrier& barrier, VkDependencyFlags dependencyFlags)
{
    VkDependencyInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    info.pNext = nullptr;
    info.dependencyFlags = dependencyFlags;
    info.memoryBarrierCount = uint32_t(barrier._memoryBarriers.size());
    info.pMemoryBarriers = barrier._memoryBarriers.data();
    info.bufferMemoryBarrierCount = uint32_t(barrier._bufferMemoryBarriers.size());
    info.pBufferMemoryBarriers = barrier._bufferMemoryBarriers.data();
    info.imageMemoryBarrierCount = uint32_t(barrier._imageMemoryBarriers.size());
    info.pImageMemoryBarriers = barrier._imageMemoryBarriers.data();

    vkCmdPipelineBarrier2(_vkCommandBuffer, &info);
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::ClearColorImage(const Instance::Image& image, VkImageLayout imageLayout, const VkClearColorValue& color)
{
    VkImageSubresourceRange range{};
    range.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = image.MipmapLevelCount();
    range.baseArrayLayer = 0;
    range.layerCount = image.LayerCount();
    
    vkCmdClearColorImage(_vkCommandBuffer, image.VkHandle(), imageLayout, &color, 1, &range);
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::CopyBufferToImage(const Instance::Buffer& buffer, const Instance::Image& image, VkImageLayout imageLayout, VkImageAspectFlags imageAspectFlags)
{
    auto&& pmlExtent = image.PerMipmapLevelExtent3D();
    auto blockByteSize = vk::blockSize(vk::Format(image.Format()));
    VkDeviceSize offset = 0;

    std::vector< VkBufferImageCopy> copys(pmlExtent.size(), VkBufferImageCopy{});
    for (uint32_t mipmapLevelIndex = 0; mipmapLevelIndex < copys.size(); mipmapLevelIndex++)
    {
        VkBufferImageCopy& copy = copys[mipmapLevelIndex];

        copy.bufferOffset = offset;
        copy.bufferRowLength = 0;
        copy.bufferImageHeight = 0;
        copy.imageSubresource.aspectMask = imageAspectFlags;
        copy.imageSubresource.baseArrayLayer = 0;
        copy.imageSubresource.layerCount = image.LayerCount();
        copy.imageSubresource.mipLevel = mipmapLevelIndex;
        copy.imageOffset = { 0, 0, 0 };
        copy.imageExtent = pmlExtent[mipmapLevelIndex];

        offset += blockByteSize * pmlExtent[mipmapLevelIndex].width * pmlExtent[mipmapLevelIndex].height * pmlExtent[mipmapLevelIndex].depth;
    }

    vkCmdCopyBufferToImage(_vkCommandBuffer, buffer.VkHandle(), image.VkHandle(), imageLayout, uint32_t(copys.size()), copys.data());
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::Blit(const Instance::Image& srcImage, VkImageLayout srcImageLayout, const Instance::Image& dstImage, VkImageLayout dstImageLayout, VkImageAspectFlags imageAspectFlags, VkFilter filter)
{
    auto&& layerCount = std::min(srcImage.LayerCount(), dstImage.LayerCount());
    auto&& mipmapLevelCount = std::min(srcImage.MipmapLevelCount(), dstImage.MipmapLevelCount());
    auto&& srcPmlExtent = srcImage.PerMipmapLevelExtent3D();
    auto&& dstPmlExtent = dstImage.PerMipmapLevelExtent3D();

    std::vector<VkImageBlit> blits(mipmapLevelCount);
    for (uint32_t i = 0; i < mipmapLevelCount; i++)
    {
        auto& blit = blits[i];
        auto&& srcExtent = srcPmlExtent[i];
        auto&& dstExtent = dstPmlExtent[i];
        blit.srcSubresource.aspectMask = imageAspectFlags;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = layerCount;
        blit.srcSubresource.mipLevel = i;
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { static_cast<int>(srcExtent.width), static_cast<int>(srcExtent.height), static_cast<int>(srcExtent.depth) };
        blit.dstSubresource.aspectMask = imageAspectFlags;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = layerCount;
        blit.dstSubresource.mipLevel = i;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { static_cast<int>(dstExtent.width), static_cast<int>(dstExtent.height), static_cast<int>(dstExtent.depth) };
    }

    vkCmdBlitImage(_vkCommandBuffer, srcImage.VkHandle(), srcImageLayout, dstImage.VkHandle(), dstImageLayout, static_cast<uint32_t>(blits.size()), blits.data(), filter);
}
