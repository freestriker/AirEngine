#include "TaskManager.hpp"
#include <iostream>

tf::Executor AirEngine::Runtime::Core::Manager::TaskManager::_executor{ std::thread::hardware_concurrency() - 1 };

AirEngine::Runtime::Core::Manager::TaskManager::TaskManager()
	: ManagerBase("TaskManager")
{
}

AirEngine::Runtime::Core::Manager::TaskManager::~TaskManager()
{
}