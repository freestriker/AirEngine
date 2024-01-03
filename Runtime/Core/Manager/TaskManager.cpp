#include "TaskManager.hpp"
#include <iostream>

tf::Executor AirEngine::Runtime::Core::Manager::TaskManager::_executor{ std::thread::hardware_concurrency() - 1 };

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Core::Manager::TaskManager::OnGetInitializeOperations()
{
	return
	{
		{
			0, 0,
			[]()->void
			{
				std::cout << "Task managers worker count is: "<< _executor.num_workers() << ". \n";
			}
		}
	};
}

AirEngine::Runtime::Core::Manager::TaskManager::TaskManager()
	: ManagerBase("TaskManager")
{
}

AirEngine::Runtime::Core::Manager::TaskManager::~TaskManager()
{
}