#include "FiberManager.hpp"
#include <iostream>
#include <sstream>
#include <array>

bool AirEngine::Runtime::Core::Manager::FiberManager::_isEnded{false};
bool AirEngine::Runtime::Core::Manager::FiberManager::_isBooted{false};
std::mutex AirEngine::Runtime::Core::Manager::FiberManager::_endMutex{  };
AirEngine::Runtime::Utility::Fiber::condition_variable_any AirEngine::Runtime::Core::Manager::FiberManager::_endConditionVariable{};
std::vector<std::thread> AirEngine::Runtime::Core::Manager::FiberManager::_workerThreads{};
AirEngine::Runtime::Utility::Fiber::fiber AirEngine::Runtime::Core::Manager::FiberManager::_mainLoopFiber{};

void AirEngine::Runtime::Core::Manager::FiberManager::BootThread()
{
	auto hardwareThreadCount{ std::thread::hardware_concurrency() - 1 };
	using namespace AirEngine::Runtime::Utility;
    auto showThreadIdTask = [](const std::string& name)->void
    {
        //std::stringstream ss{};
        //ss << name << ": " << std::this_thread::get_id() << ".\n";
        //std::cout << ss.str();
    };
    auto mainLoopTask = [showThreadIdTask]()->void
    {
        while (true)
        {
            ThisFiber::yield();
            showThreadIdTask("MainLoop");
            std::array< Fiber::fiber, 40> perFrameTasks{};
            for (int i = 0; i < perFrameTasks.size(); i++)
            {
                perFrameTasks[i] = Fiber::fiber(showThreadIdTask, std::to_string(i));
            }
            ThisFiber::yield();
            for (int i = 0; i < perFrameTasks.size(); i++)
            {
                perFrameTasks[i].join();
            }                      
        }
    };
    auto worker = [hardwareThreadCount, mainLoopTask](uint32_t workerIndex)->void
	{
		if (workerIndex == 0) _mainLoopFiber = std::move(Fiber::fiber(mainLoopTask));

		Fiber::use_scheduling_algorithm< Fiber::algo::work_stealing >(hardwareThreadCount);
		std::unique_lock<std::mutex> lock(_endMutex);
		_endConditionVariable.wait(lock, []() { return _isEnded; });
	};
	_workerThreads.reserve(hardwareThreadCount);
	for (uint32_t workerIndex = 0; workerIndex < hardwareThreadCount; ++workerIndex)
	{
		_workerThreads.emplace_back(worker, workerIndex);
	}
}

std::vector<AirEngine::Runtime::Core::Boot::ManagerInitializerWrapper> AirEngine::Runtime::Core::Manager::FiberManager::OnGetManagerInitializers()
{
	return
	{
        {
            std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max(),
            BootThread
        }
	};
}

void AirEngine::Runtime::Core::Manager::FiberManager::OnFinishInitialize()
{
	std::cout << "Finish initialize " << Name() << std::endl;
}


AirEngine::Runtime::Core::Manager::FiberManager::FiberManager()
	: ManagerBase("FiberManager")
{
}

AirEngine::Runtime::Core::Manager::FiberManager::~FiberManager()
{
}