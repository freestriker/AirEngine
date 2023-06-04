﻿#include "DummyWindow.hpp"
#include "../Manager/GraphicDeviceManager.hpp"


void AirEngine::Runtime::Core::FrontEnd::DummyWindow::OnCreate()
{
}

void AirEngine::Runtime::Core::FrontEnd::DummyWindow::OnAcquireImage()
{
	Utility::ThisFiber::sleep_for(std::chrono::seconds(1));
}

void AirEngine::Runtime::Core::FrontEnd::DummyWindow::OnPresent()
{
}

AirEngine::Runtime::Core::FrontEnd::DummyWindow::DummyWindow()
	: FrontEndBase()
{
}
