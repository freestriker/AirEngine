#include <iostream>
#include <sstream>
#include <array>
#include "RenderManager.hpp"
#include "GraphicDeviceManager.hpp"
#include "../FrontEnd/Window.hpp"
#include "../FrontEnd/DummyWindow.hpp"

AirEngine::Runtime::Core::FrontEnd::FrontEndBase* AirEngine::Runtime::Core::Manager::RenderManager::_frontEnd{nullptr};

void AirEngine::Runtime::Core::Manager::RenderManager::CreateMainWindow()
{
	//_frontEnd = new FrontEnd::DummyWindow();
	_frontEnd = new FrontEnd::Window();
	if (_frontEnd->IsWindow())
	{
		dynamic_cast<FrontEnd::WindowFrontEndBase*>(_frontEnd)->OnSetVulkanHandle();
	}
}

void AirEngine::Runtime::Core::Manager::RenderManager::CreateSwapchain()
{
	if (_frontEnd->IsWindow())
	{
		dynamic_cast<FrontEnd::WindowFrontEndBase*>(_frontEnd)->OnCreateVulkanSwapchain();
	}
}

std::vector<AirEngine::Runtime::Core::Boot::ManagerInitializerWrapper> AirEngine::Runtime::Core::Manager::RenderManager::OnGetManagerInitializers()
{
	return
	{
        { 0, 1, CreateMainWindow }, 
        { 0, 3, CreateSwapchain }
	};
}

void AirEngine::Runtime::Core::Manager::RenderManager::OnFinishInitialize()
{
	std::cout << "Finish initialize " << Name() << std::endl;
}


AirEngine::Runtime::Core::Manager::RenderManager::RenderManager()
	: ManagerBase("RenderManager")
{
}

AirEngine::Runtime::Core::Manager::RenderManager::~RenderManager()
{
}