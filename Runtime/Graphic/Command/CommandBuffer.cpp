#include "CommandBuffer.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"
#include "CommandPool.hpp"
#include "Barrier.hpp"
#include "../Instance/Image.hpp"

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