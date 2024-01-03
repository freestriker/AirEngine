#include <iostream>
#include "LogicManager.hpp"
#include "RenderManager.hpp"
#include "FiberManager.hpp"

AirEngine::Runtime::Utility::Fiber::fiber AirEngine::Runtime::Core::Manager::LogicManager::_logicLoopFiber{ };

void AirEngine::Runtime::Core::Manager::LogicManager::AddLogicLoop()
{
    FiberManager::AddFiberInitializers({
        []()->void
        {
			_logicLoopFiber = std::move(Utility::Fiber::fiber(LogicLoop));
		}
	});
}

void AirEngine::Runtime::Core::Manager::LogicManager::LogicLoop()
{
	while (true)
	{
		//logic
		std::cout << "Logic loop.\n";
		Utility::ThisFiber::sleep_for(std::chrono::milliseconds(5));
		
		if (RenderManager::TryBeginRender())
		{
			RenderManager::EndRender();
		}
    }
}

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Core::Manager::LogicManager::OnGetInitializeOperations()
{
	return
	{
        { 1, 0, AddLogicLoop }
	};
}

AirEngine::Runtime::Core::Manager::LogicManager::LogicManager()
	: ManagerBase("LogicManager")
{
}

AirEngine::Runtime::Core::Manager::LogicManager::~LogicManager()
{
}