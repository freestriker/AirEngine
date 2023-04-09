#include "Semaphore.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"

AirEngine::Runtime::Graphic::Command::Semaphore::Semaphore()
	: _vkSemaphore(VK_NULL_HANDLE)
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	auto result = vkCreateSemaphore(Core::Manager::GraphicDeviceManager::VkDevice(), &semaphoreInfo, nullptr, &_vkSemaphore);
	if(result != VK_SUCCESS) qFatal("Failed to create semaphore.");
}

AirEngine::Runtime::Graphic::Command::Semaphore::~Semaphore()
{
	vkDestroySemaphore(Core::Manager::GraphicDeviceManager::VkDevice(), _vkSemaphore, nullptr);
}