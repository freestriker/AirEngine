#include "Semaphore.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"

AirEngine::Runtime::Graphic::Command::Semaphore::Semaphore()
	: _vkSemaphore(Graphic::Manager::DeviceManager::Device().createSemaphore(vk::SemaphoreCreateInfo{}))
{
}

AirEngine::Runtime::Graphic::Command::Semaphore::~Semaphore()
{
	Graphic::Manager::DeviceManager::Device().destroySemaphore(_vkSemaphore);
}