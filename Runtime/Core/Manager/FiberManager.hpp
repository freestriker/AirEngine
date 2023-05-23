#pragma once
#include "ManagerBase.hpp"
#include <memory>
#include "../../Utility/Initializer.hpp"
#include <mutex>
#include "../../Utility/Fiber.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Manager
			{
				class AIR_ENGINE_API FiberManager
					: public ManagerBase
				{
				private:
					NO_COPY_MOVE(FiberManager)
				protected:
					virtual std::vector<Utility::InitializerWrapper> OnGetManagerInitializers() override;
					virtual void OnFinishInitialize() override;
					static void BootThread();
				public:
					FiberManager();
					virtual ~FiberManager();
				protected:
					static std::vector<Utility::Initializer> _fiberInitializers;
					static bool _isEnded;
					static bool _isBooted;
					static std::mutex _endMutex;
					static AirEngine::Runtime::Utility::Fiber::condition_variable_any _endConditionVariable;
					static std::vector<std::thread> _workerThreads;
					static AirEngine::Runtime::Utility::Fiber::fiber _mainLoopFiber;
				public:
					static void AddFiberInitializers(const std::vector<Utility::Initializer>& initializer);
				};
			}
		}
	}
}