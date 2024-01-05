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
    _frontEnd->OnCreateSurface();
}

void AirEngine::Runtime::Core::Manager::RenderManager::CreateSwapchain()
{
	_frontEnd->OnCreateSwapchain();
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
        { GRAPHIC_INITIALIZE_LAYER, GRAPHIC_INITIALIZE_WINDOW_SURFACE_INDEX, CreateMainWindow },
        { GRAPHIC_INITIALIZE_LAYER, GRAPHIC_INITIALIZE_SWAPCHAIN_INDEX, CreateSwapchain },
	};
}

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Core::Manager::RenderManager::OnGetUpdateOperations()
{
	return
	{
		{ RENDER_UPDATE_LAYER, 0, RenderUpdate }
	};
}

AirEngine::Runtime::Core::Manager::RenderManager::RenderManager()
	: ManagerBase("RenderManager")
{
}

AirEngine::Runtime::Core::Manager::RenderManager::~RenderManager()
{
}
