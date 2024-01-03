#include "FiberManager.hpp"
#include <iostream>
#include <sstream>
#include <array>

std::vector<AirEngine::Runtime::Utility::Operation> AirEngine::Runtime::Core::Manager::FiberManager::_fiberInitializers{ };
bool AirEngine::Runtime::Core::Manager::FiberManager::_isEnded{false};
bool AirEngine::Runtime::Core::Manager::FiberManager::_isBooted{false};
std::mutex AirEngine::Runtime::Core::Manager::FiberManager::_endMutex{  };
AirEngine::Runtime::Utility::Fiber::condition_variable_any AirEngine::Runtime::Core::Manager::FiberManager::_endConditionVariable{};
std::vector<std::thread> AirEngine::Runtime::Core::Manager::FiberManager::_workerThreads{};

void AirEngine::Runtime::Core::Manager::FiberManager::BootThread()
{
	auto hardwareThreadCount{ std::thread::hardware_concurrency() - 1 };
	using namespace AirEngine::Runtime::Utility;
    auto worker = [hardwareThreadCount](uint32_t workerIndex)->void
	{
		if (workerIndex == 0) 
        {
			int n = _fiberInitializers.size();
            for (auto& _initializer : _fiberInitializers)
            {
                _initializer();
            }
            _fiberInitializers.clear();
        }

		Fiber::use_scheduling_algorithm< Fiber::algo::work_stealing >(hardwareThreadCount, true);
		std::unique_lock<std::mutex> lock(_endMutex);
		_endConditionVariable.wait(lock, []() { return _isEnded; });
	};
	_workerThreads.reserve(hardwareThreadCount);
	for (uint32_t workerIndex = 0; workerIndex < hardwareThreadCount; ++workerIndex)
	{
		_workerThreads.emplace_back(worker, workerIndex);
	}
}

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Core::Manager::FiberManager::OnGetInitializeOperations()
{
	return
	{
        {
            std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max(),
            BootThread
        }
	};
}

AirEngine::Runtime::Core::Manager::FiberManager::FiberManager()
	: ManagerBase("FiberManager")
{
}

AirEngine::Runtime::Core::Manager::FiberManager::~FiberManager()
{
}

void AirEngine::Runtime::Core::Manager::FiberManager::AddFiberInitializers(const std::vector<Utility::Operation>& initializer)
{
    _fiberInitializers.insert(_fiberInitializers.end(), initializer.begin(), initializer.end());
}
