#include <iostream>
#include <sstream>
#include <array>
#include "RenderManager.hpp"
#include "GraphicDeviceManager.hpp"
#include "AirEngine/Runtime/Core/FrontEnd/Window.hpp"
#include "AirEngine/Runtime/Core/FrontEnd/DummyWindow.hpp"

AirEngine::Runtime::Core::FrontEnd::FrontEndBase* AirEngine::Runtime::Core::Manager::RenderManager::_frontEnd{nullptr};
//AirEngine::Runtime::Core::Manager::RenderManager::Status AirEngine::Runtime::Core::Manager::RenderManager::_status{ AirEngine::Runtime::Core::Manager::RenderManager::Status::NONE};
//AirEngine::Runtime::Utility::Condition<std::mutex, std::condition_variable> AirEngine::Runtime::Core::Manager::RenderManager::_beginRenderCondition{ };
//AirEngine::Runtime::Utility::Condition<std::mutex, std::condition_variable> AirEngine::Runtime::Core::Manager::RenderManager::_endPresentCondition{ };

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

void AirEngine::Runtime::Core::Manager::RenderManager::RenderUpdate()
{
	//_status = Status::ACQUIRE;
	_frontEnd->OnAcquireImage();

	//_status = Status::READY;
	//_beginRenderCondition.Wait();
	//_beginRenderCondition.Reset();

	//_status = Status::RENDERING;
	//render
	std::cout << "Render loop.\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(5));

	//_status = Status::PRESENT;
	_frontEnd->OnPresent();

	//_endPresentCondition.Awake();
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
//
//bool AirEngine::Runtime::Core::Manager::RenderManager::TryBeginRender()
//{
//	if (_status == Status::READY)
//	{
//		_beginRenderCondition.Awake();
//		return true;
//	}
//	return false;
//}
//
//void AirEngine::Runtime::Core::Manager::RenderManager::EndRender()
//{
//	_endPresentCondition.Wait();
//	_endPresentCondition.Reset();
//}
