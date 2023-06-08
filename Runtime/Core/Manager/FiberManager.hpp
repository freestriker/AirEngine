#pragma once
#include "../../Utility/ThreadInclude.hpp"
#include "ManagerBase.hpp"
#include <memory>
#include "../../Utility/Initializer.hpp"
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
					virtual std::vector<Utility::InitializerWrapper> OnGetInternalInitializers() override;
					static void BootThread();
				public:
					FiberManager();
					virtual ~FiberManager();
				protected:
					static std::vector<Utility::Operation> _fiberInitializers;
					static bool _isEnded;
					static bool _isBooted;
					static std::mutex _endMutex;
					static AirEngine::Runtime::Utility::Fiber::condition_variable_any _endConditionVariable;
					static std::vector<std::thread> _workerThreads;
				public:
					static void AddFiberInitializers(const std::vector<Utility::Operation>& initializer);
				};
			}
		}
	}
}