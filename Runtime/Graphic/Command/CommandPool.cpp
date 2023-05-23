#include "CommandPool.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"

AirEngine::Runtime::Graphic::Command::CommandPool::CommandPool(const Utility::InternedString queueName, VkCommandPoolCreateFlags flags)
	: _vkCommandPool(VK_NULL_HANDLE)
	, _queue(&Core::Manager::GraphicDeviceManager::Queue(queueName))
	, _flags(flags)
{
	VkCommandPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	createInfo.flags = flags;
	createInfo.queueFamilyIndex = _queue->FamilyIndex();

	auto result = vkCreateCommandPool(Core::Manager::GraphicDeviceManager::VkDevice(), &createInfo, nullptr, &_vkCommandPool);
	if (result != VK_SUCCESS) qFatal("Failed to create command pool.");
}

AirEngine::Runtime::Graphic::Command::CommandPool::~CommandPool()
{
	vkDestroyCommandPool(Core::Manager::GraphicDeviceManager::VkDevice(), _vkCommandPool, nullptr);
}
