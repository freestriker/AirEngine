#include <iostream>
#include <sstream>
#include <array>
#include "RenderManager.hpp"
#include "GraphicDeviceManager.hpp"
#include "../FrontEnd/Window.hpp"
#include "../FrontEnd/DummyWindow.hpp"
#include "FiberManager.hpp"

AirEngine::Runtime::Core::FrontEnd::FrontEndBase* AirEngine::Runtime::Core::Manager::RenderManager::_frontEnd{nullptr};
AirEngine::Runtime::Utility::Fiber::fiber AirEngine::Runtime::Core::Manager::RenderManager::_renderLoopFiber{ };

void AirEngine::Runtime::Core::Manager::RenderManager::CreateMainWindow()
{
	_frontEnd = new FrontEnd::DummyWindow();
	//_frontEnd = new FrontEnd::Window();
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

void AirEngine::Runtime::Core::Manager::RenderManager::AddRenderLoop()
{
    FiberManager::AddFiberInitializers({
        []()->void
        {
            _renderLoopFiber = std::move(Utility::Fiber::fiber(RenderLoop));
}
        });
   
}

void AirEngine::Runtime::Core::Manager::RenderManager::RenderLoop()
{
	while (true)
	{
        using namespace AirEngine::Runtime::Utility;
        
        auto threadId0 = std::this_thread::get_id();
        int i = 2;
        i = i * 3;
        ThisFiber::yield();
        auto threadId1 = std::this_thread::get_id();
        //if (threadId0 != threadId1) std::cout << "Change thread1." << std::endl;
        i = i + 2;
        i = i * 3;
        ThisFiber::yield();
        auto threadId2 = std::this_thread::get_id();
        //if (threadId0 != threadId2) std::cout << "Change thread2." << std::endl;
        i = i + 2;
        i = i * 3;
        ThisFiber::yield();
        auto threadId3 = std::this_thread::get_id();
        //if (threadId0 != threadId3) std::cout << "Change thread3." << std::endl;
        i = i + 2;
        i = i * 3;
        ThisFiber::yield();
        std::cout << "Render loop.\n";
    }
}

std::vector<AirEngine::Runtime::Utility::InitializerWrapper> AirEngine::Runtime::Core::Manager::RenderManager::OnGetManagerInitializers()
{
	return
	{
        { 0, 1, CreateMainWindow }, 
        { 0, 3, CreateSwapchain },
        { 1, 0, AddRenderLoop }
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