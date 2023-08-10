#include "CommandPool.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"
#include "CommandBuffer.hpp"

AirEngine::Runtime::Graphic::Command::CommandPool::CommandPool(const Utility::InternedString queueName, vk::CommandPoolCreateFlags flags)
	: _vkCommandPool()
	, _queue(&Core::Manager::GraphicDeviceManager::Queue(queueName))
	, _flags(flags)
{
	_vkCommandPool = Core::Manager::GraphicDeviceManager::Device().createCommandPool(vk::CommandPoolCreateInfo(flags, _queue->FamilyIndex()));
}

AirEngine::Runtime::Graphic::Command::CommandPool::~CommandPool()
{
	_commandBufferMap.clear();
	Core::Manager::GraphicDeviceManager::Device().destroyCommandPool(_vkCommandPool);
}

void AirEngine::Runtime::Graphic::Command::CommandPool::Reset()
{
	Core::Manager::GraphicDeviceManager::Device().resetCommandPool(_vkCommandPool, vk::CommandPoolResetFlagBits::eReleaseResources);
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
