﻿#include <iostream>
#include "LogicManager.hpp"
#include "RenderManager.hpp"

void AirEngine::Runtime::Core::Manager::LogicManager::LogicUpdate()
{
	std::cout << "Logic loop.\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Core::Manager::LogicManager::OnGetUpdateOperations()
{
	return
	{
        { 0, 0, LogicUpdate }
	};
}

AirEngine::Runtime::Core::Manager::LogicManager::LogicManager()
	: ManagerBase("LogicManager")
{
}

AirEngine::Runtime::Core::Manager::LogicManager::~LogicManager()
{
}