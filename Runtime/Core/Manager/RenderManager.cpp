#include <iostream>
#include <sstream>
#include <array>
#include "RenderManager.hpp"
#include "GraphicDeviceManager.hpp"
#include "AirEngine/Runtime/Core/FrontEnd/Window.hpp"
#include "AirEngine/Runtime/Core/FrontEnd/DummyWindow.hpp"

AirEngine::Runtime::Core::FrontEnd::FrontEndBase* AirEngine::Runtime::Core::Manager::RenderManager::_frontEnd{nullptr};

void AirEngine::Runtime::Core::Manager::RenderManager::CreateMainWindow()
{
	//_frontEnd = new FrontEnd::DummyWindow();
	_frontEnd = new FrontEnd::Window();
    _frontEnd->OnCreate();
}

void AirEngine::Runtime::Core::Manager::RenderManager::CreateSwapchain()
{
	if (_frontEnd->IsWindow())
	{
		dynamic_cast<FrontEnd::WindowFrontEndBase*>(_frontEnd)->OnCreateVulkanSwapchain();
	}
}

void AirEngine::Runtime::Core::Manager::RenderManager::RenderUpdate()
{
	if (!_frontEnd->IsReadyToRender())
	{
		return;
	}

	_frontEnd->StartRender();

	//render
	std::cout << "Render loop.\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(5));

	_frontEnd->FinishRender();
}

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Core::Manager::RenderManager::OnGetInitializeOperations()
{
	return
	{
        { 0, 1, CreateMainWindow }, 
        { 0, 5, CreateSwapchain },
	};
}

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Core::Manager::RenderManager::OnGetUpdateOperations()
{
	return
	{
		{ 0, 1, RenderUpdate }
	};
}

AirEngine::Runtime::Core::Manager::RenderManager::RenderManager()
	: ManagerBase("RenderManager")
{
}

AirEngine::Runtime::Core::Manager::RenderManager::~RenderManager()
{
}
