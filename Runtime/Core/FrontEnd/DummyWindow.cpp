#include "DummyWindow.hpp"
#include "AirEngine/Runtime/Core/Manager/RenderManager.hpp"


void AirEngine::Runtime::Core::FrontEnd::DummyWindow::OnCreateSurface()
{
}

void AirEngine::Runtime::Core::FrontEnd::DummyWindow::OnCreateSwapchain()
{
}

void AirEngine::Runtime::Core::FrontEnd::DummyWindow::OnFinishRender()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	ReadyToRender();
}

void AirEngine::Runtime::Core::FrontEnd::DummyWindow::OnStartRender()
{
}

AirEngine::Runtime::Core::FrontEnd::DummyWindow::DummyWindow()
	: FrontEndBase()
{
}
