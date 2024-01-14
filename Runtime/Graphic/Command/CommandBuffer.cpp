#include "CommandBuffer.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"
#include "CommandPool.hpp"
#include "Barrier.hpp"
#include "AirEngine/Runtime/Graphic/Instance/Image.hpp"
#include "AirEngine/Runtime/Graphic/Instance/Buffer.hpp"
#include "AirEngine/Runtime/Graphic/Instance/FrameBuffer.hpp"
#include "AirEngine/Runtime/Graphic/Instance/RenderPassBase.hpp"
#include <vulkan/vulkan_format_traits.hpp>
#include "AirEngine/Runtime/Graphic/Rendering/Material.hpp"
#include "AirEngine/Runtime/Graphic/Rendering/Shader.hpp"

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

    _vkCommandBuffer = Graphic::Manager::DeviceManager::Device().allocateCommandBuffers(allocInfo).at(0);
}

AirEngine::Runtime::Graphic::Command::CommandBuffer::~CommandBuffer()
{
    Graphic::Manager::DeviceManager::Device().freeCommandBuffers(_commandPool->VkHandle(), _vkCommandBuffer);
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
    range.aspectMask = vk::ImageAspectFlagBits::eColor;
    
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

void AirEngine::Runtime::Graphic::Command::CommandBuffer::FillBuffer(const Instance::Buffer* buffer, size_t offset, size_t size, uint32_t data)
{
    _vkCommandBuffer.fillBuffer(buffer->VkHandle(), offset, size, data);
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

void AirEngine::Runtime::Graphic::Command::CommandBuffer::BindDsecriptorBuffer(const Instance::Buffer* descriptorBuffer)
{
    vk::DescriptorBufferBindingInfoEXT descriptorBufferBindingInfo(descriptorBuffer->BufferDeviceAddress(), vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT | vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT);
    _vkCommandBuffer.bindDescriptorBuffersEXT(descriptorBufferBindingInfo);
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::BindMaterial(const Rendering::Material* material, Utility::InternedString subpassName)
{
    auto subShaderInfoIter = material->Shader()->Info().subShaderInfoMap.find(subpassName);
    if (subShaderInfoIter == material->Shader()->Info().subShaderInfoMap.end()) qFatal("This shader do not contain the same name of subShader.");

    const auto pipelineLayout = subShaderInfoIter->second.pipelineLayout;
    const auto pipelineBindPoint = material->Shader()->Info().pipelineBindPoint;
    const auto& descriptorSetMemoryInfos = material->DescriptorSetMemoryInfosMap().at(subpassName);
    const auto descriptorBufferIndexs = std::vector<uint32_t>(descriptorSetMemoryInfos.size(), 0);
    auto descriptorBufferOffsets = std::vector<vk::DeviceAddress>(descriptorSetMemoryInfos.size());
    for (uint32_t i = 0; i < descriptorSetMemoryInfos.size(); ++i)
    {
        descriptorBufferOffsets.at(i) = descriptorSetMemoryInfos.at(i).handle.Offset();
    }

    _vkCommandBuffer.setDescriptorBufferOffsetsEXT(pipelineBindPoint, pipelineLayout, 0, descriptorBufferIndexs, descriptorBufferOffsets);
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::BeginRenderPass(Graphic::Instance::RenderPassBase* renderPass, Graphic::Instance::FrameBuffer* frameBuffer, const std::vector<vk::ClearValue>& clearValues)
{
    if (renderPass == nullptr || frameBuffer == nullptr) qFatal("Begin render pass must have valid data.");

    auto&& renderRect = vk::Rect2D({ 0, 0 }, frameBuffer->Extent2D());
    auto&& renderViewport = vk::Viewport(renderRect.offset.x, renderRect.offset.y, renderRect.extent.width, renderRect.extent.height, 0, 1);

    std::vector<vk::ClearValue> values(clearValues);
    values.resize(renderPass->Info().FullAttachmentInfoMap().size(), vk::ClearValue());

    vk::RenderPassBeginInfo renderPassBeginInfo(renderPass->VkHandle(), frameBuffer->VkHandle(), renderRect, values);

    _vkCommandBuffer.beginRenderPass(renderPassBeginInfo, _vkCommandBufferLevel == vk::CommandBufferLevel::ePrimary ? vk::SubpassContents::eInline : vk::SubpassContents::eSecondaryCommandBuffers);
    _vkCommandBuffer.setViewport(0, 1, &renderViewport);
    _vkCommandBuffer.setScissor(0, 1, &renderRect);
}

void AirEngine::Runtime::Graphic::Command::CommandBuffer::EndRenderPass()
{
    _vkCommandBuffer.endRenderPass();
}
