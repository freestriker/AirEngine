#include "Fence.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"

AirEngine::Runtime::Graphic::Command::Fence::Fence()
	: Fence(false)
{
}

AirEngine::Runtime::Graphic::Command::Fence::Fence(bool isSignaled)
    : _vkFence(Graphic::Manager::DeviceManager::Device().createFence(vk::FenceCreateInfo(isSignaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags{ 0 })))
{
}

AirEngine::Runtime::Graphic::Command::Fence::~Fence()
{
    Graphic::Manager::DeviceManager::Device().destroyFence(_vkFence);
}

void AirEngine::Runtime::Graphic::Command::Fence::Reset() const
{
    Graphic::Manager::DeviceManager::Device().resetFences(_vkFence);
}

void AirEngine::Runtime::Graphic::Command::Fence::Wait() const
{
    Graphic::Manager::DeviceManager::Device().waitForFences(_vkFence, true, std::numeric_limits<uint64_t>::max());
}

vk::Result AirEngine::Runtime::Graphic::Command::Fence::Status() const
{
    return Graphic::Manager::DeviceManager::Device().getFenceStatus(_vkFence);
}
