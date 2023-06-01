#include "Fence.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"

AirEngine::Runtime::Graphic::Command::Fence::Fence()
	: Fence(false)
{
}

AirEngine::Runtime::Graphic::Command::Fence::Fence(bool isSignaled)
{
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = isSignaled ? VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT : 0;
    auto result = vkCreateFence(Core::Manager::GraphicDeviceManager::VkDevice(), &fenceInfo, nullptr, &_vkFence);
    if (result != VK_SUCCESS) qFatal("Failed to create fence.");
}

AirEngine::Runtime::Graphic::Command::Fence::~Fence()
{
    vkDestroyFence(Core::Manager::GraphicDeviceManager::VkDevice(), _vkFence, nullptr);
}

void AirEngine::Runtime::Graphic::Command::Fence::Reset() const
{
    auto result = vkResetFences(Core::Manager::GraphicDeviceManager::VkDevice(), 1, &_vkFence);
    if (result != VK_SUCCESS) qFatal("Failed to reset fence.");
}

void AirEngine::Runtime::Graphic::Command::Fence::Wait() const
{
    auto result = vkWaitForFences(Core::Manager::GraphicDeviceManager::VkDevice(), 1, &_vkFence, VK_TRUE, UINT64_MAX);
    if (result != VK_SUCCESS) qFatal("Failed to wait fence.");
}

VkResult AirEngine::Runtime::Graphic::Command::Fence::Status() const
{
    return vkGetFenceStatus(Core::Manager::GraphicDeviceManager::VkDevice(), _vkFence);
}
