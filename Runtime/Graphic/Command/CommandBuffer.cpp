#include "CommandBuffer.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"
#include "CommandPool.hpp"

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
