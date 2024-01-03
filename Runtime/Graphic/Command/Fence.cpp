#include "Fence.hpp"
#include "AirEngine/Runtime/Core/Manager/GraphicDeviceManager.hpp"

AirEngine::Runtime::Graphic::Command::Fence::Fence()
	: Fence(false)
{
}

AirEngine::Runtime::Graphic::Command::Fence::Fence(bool isSignaled)
    : _vkFence(Core::Manager::GraphicDeviceManager::Device().createFence(vk::FenceCreateInfo(isSignaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags{ 0 })))
{
}

AirEngine::Runtime::Graphic::Command::Fence::~Fence()
{
    Core::Manager::GraphicDeviceManager::Device().destroyFence(_vkFence);
}

void AirEngine::Runtime::Graphic::Command::Fence::Reset() const
{
    Core::Manager::GraphicDeviceManager::Device().resetFences(_vkFence);
}

void AirEngine::Runtime::Graphic::Command::Fence::Wait() const
{
    Core::Manager::GraphicDeviceManager::Device().waitForFences(_vkFence, true, std::numeric_limits<uint64_t>::max());
}

vk::Result AirEngine::Runtime::Graphic::Command::Fence::Status() const
{
    return Core::Manager::GraphicDeviceManager::Device().getFenceStatus(_vkFence);
}
