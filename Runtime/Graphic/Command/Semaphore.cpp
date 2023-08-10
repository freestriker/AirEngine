#include "Semaphore.hpp"
#include "../../Core/Manager/GraphicDeviceManager.hpp"

AirEngine::Runtime::Graphic::Command::Semaphore::Semaphore()
	: _vkSemaphore(Core::Manager::GraphicDeviceManager::Device().createSemaphore(vk::SemaphoreCreateInfo{}))
{
}

AirEngine::Runtime::Graphic::Command::Semaphore::~Semaphore()
{
	Core::Manager::GraphicDeviceManager::Device().destroySemaphore(_vkSemaphore);
}