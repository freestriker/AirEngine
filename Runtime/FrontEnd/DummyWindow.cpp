#include "DummyWindow.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"


void AirEngine::Runtime::FrontEnd::DummyWindow::OnCreateSurface()
{
}

void AirEngine::Runtime::FrontEnd::DummyWindow::OnCreateSwapchain()
{
}

void AirEngine::Runtime::FrontEnd::DummyWindow::OnFinishRender()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	ReadyToRender();
}

void AirEngine::Runtime::FrontEnd::DummyWindow::OnStartRender()
{
}

AirEngine::Runtime::FrontEnd::DummyWindow::DummyWindow()
	: FrontEndBase()
{
}
