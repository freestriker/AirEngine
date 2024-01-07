#include "CommandPool.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"
#include "CommandBuffer.hpp"

AirEngine::Runtime::Graphic::Command::CommandPool::CommandPool(const Utility::InternedString queueName, vk::CommandPoolCreateFlags flags)
	: _vkCommandPool()
	, _queue(&Graphic::Manager::DeviceManager::Queue(queueName))
	, _flags(flags)
{
	_vkCommandPool = Graphic::Manager::DeviceManager::Device().createCommandPool(vk::CommandPoolCreateInfo(flags, _queue->FamilyIndex()));
}

AirEngine::Runtime::Graphic::Command::CommandPool::~CommandPool()
{
	_commandBufferMap.clear();
	Graphic::Manager::DeviceManager::Device().destroyCommandPool(_vkCommandPool);
}

void AirEngine::Runtime::Graphic::Command::CommandPool::Reset()
{
	Graphic::Manager::DeviceManager::Device().resetCommandPool(_vkCommandPool, vk::CommandPoolResetFlagBits::eReleaseResources);
}

AirEngine::Runtime::Graphic::Command::CommandBuffer& AirEngine::Runtime::Graphic::Command::CommandPool::CreateCommandBuffer(Utility::InternedString commandBufferName, vk::CommandBufferLevel level)
{
	auto&& commandBuffer = new Command::CommandBuffer(commandBufferName, this, level);
	_commandBufferMap.insert(std::make_pair(commandBufferName, std::move(commandBuffer)));
	return *commandBuffer;
}

void AirEngine::Runtime::Graphic::Command::CommandPool::DestroyCommandBuffer(Utility::InternedString commandBufferName)
{
	_commandBufferMap.erase(commandBufferName);
}
