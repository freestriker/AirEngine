#include "Fence.hpp"
#include "AirEngine/Runtime/Core/Manager/RenderManager.hpp"

AirEngine::Runtime::Graphic::Command::Fence::Fence()
	: Fence(false)
{
}

AirEngine::Runtime::Graphic::Command::Fence::Fence(bool isSignaled)
    : _vkFence(Core::Manager::RenderManager::Device().createFence(vk::FenceCreateInfo(isSignaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags{ 0 })))
{
}

AirEngine::Runtime::Graphic::Command::Fence::~Fence()
{
    Core::Manager::RenderManager::Device().destroyFence(_vkFence);
}

void AirEngine::Runtime::Graphic::Command::Fence::Reset() const
{
    Core::Manager::RenderManager::Device().resetFences(_vkFence);
}

void AirEngine::Runtime::Graphic::Command::Fence::Wait() const
{
    Core::Manager::RenderManager::Device().waitForFences(_vkFence, true, std::numeric_limits<uint64_t>::max());
}

vk::Result AirEngine::Runtime::Graphic::Command::Fence::Status() const
{
    return Core::Manager::RenderManager::Device().getFenceStatus(_vkFence);
}
