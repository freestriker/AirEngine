#include <iostream>
#include <sstream>
#include <array>
#include "RenderManager.hpp"
#include "GraphicDeviceManager.hpp"
#include "../FrontEnd/Window.hpp"
#include "../FrontEnd/DummyWindow.hpp"
#include "FiberManager.hpp"

AirEngine::Runtime::Core::FrontEnd::FrontEndBase* AirEngine::Runtime::Core::Manager::RenderManager::_frontEnd{nullptr};
AirEngine::Runtime::Core::Manager::RenderManager::Status AirEngine::Runtime::Core::Manager::RenderManager::_status{ AirEngine::Runtime::Core::Manager::RenderManager::Status::NONE};
AirEngine::Runtime::Utility::Fiber::fiber AirEngine::Runtime::Core::Manager::RenderManager::_renderLoopFiber{ };
AirEngine::Runtime::Utility::Condition<AirEngine::Runtime::Utility::Fiber::mutex, AirEngine::Runtime::Utility::Fiber::condition_variable> AirEngine::Runtime::Core::Manager::RenderManager::_beginRenderCondition{ };
AirEngine::Runtime::Utility::Condition<AirEngine::Runtime::Utility::Fiber::mutex, AirEngine::Runtime::Utility::Fiber::condition_variable> AirEngine::Runtime::Core::Manager::RenderManager::_endPresentCondition{ };

void AirEngine::Runtime::Core::Manager::RenderManager::CreateMainWindow()
{
	//_frontEnd = new FrontEnd::DummyWindow();
	_frontEnd = new FrontEnd::Window();
    _frontEnd->OnCreate();
	if (_frontEnd->IsWindow())
	{
		dynamic_cast<FrontEnd::WindowFrontEndBase*>(_frontEnd)->OnSetVulkanHandle();
	}
}

void AirEngine::Runtime::Core::Manager::RenderManager::CreateSwapchain()
{
	if (_frontEnd->IsWindow())
	{
		dynamic_cast<FrontEnd::WindowFrontEndBase*>(_frontEnd)->OnRecreateVulkanSwapchain();
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
		_status = Status::ACQUIRE;
        _frontEnd->OnAcquireImage();

		_status = Status::READY;
		_beginRenderCondition.Wait();
		_beginRenderCondition.Reset();

		_status = Status::RENDERING;
		//render
		std::cout << "Render loop.\n";
		Utility::ThisFiber::sleep_for(std::chrono::milliseconds(5));

		_status = Status::PRESENT;
		_frontEnd->OnPresent();

		_endPresentCondition.Awake();
    }
}

std::vector<AirEngine::Runtime::Utility::InitializerWrapper> AirEngine::Runtime::Core::Manager::RenderManager::OnGetInternalInitializers()
{
	return
	{
        { 0, 1, CreateMainWindow }, 
        { 0, 5, CreateSwapchain },
        { 1, 0, AddRenderLoop }
	};
}

AirEngine::Runtime::Core::Manager::RenderManager::RenderManager()
	: ManagerBase("RenderManager")
{
}

AirEngine::Runtime::Core::Manager::RenderManager::~RenderManager()
{
}

bool AirEngine::Runtime::Core::Manager::RenderManager::TryBeginRender()
{
	if (_status == Status::READY)
	{
		_beginRenderCondition.Awake();
		return true;
	}
	return false;
}

void AirEngine::Runtime::Core::Manager::RenderManager::EndRender()
{
	_endPresentCondition.Wait();
	_endPresentCondition.Reset();
}
