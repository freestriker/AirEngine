#include "Semaphore.hpp"
#include "AirEngine/Runtime/Core/Manager/RenderManager.hpp"

AirEngine::Runtime::Graphic::Command::Semaphore::Semaphore()
	: _vkSemaphore(Core::Manager::RenderManager::Device().createSemaphore(vk::SemaphoreCreateInfo{}))
{
}

AirEngine::Runtime::Graphic::Command::Semaphore::~Semaphore()
{
	Core::Manager::RenderManager::Device().destroySemaphore(_vkSemaphore);
}