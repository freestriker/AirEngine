#pragma once
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
				class AIR_ENGINE_API LogicManager
					: public ManagerBase
				{
				private:
					NO_COPY_MOVE(LogicManager)
					static void AddLogicLoop();
					static void LogicLoop();
					static Utility::Fiber::fiber _logicLoopFiber;
				protected:
					virtual std::vector<Utility::InitializerWrapper> OnGetInternalInitializers() override;
				public:
					LogicManager();
					virtual ~LogicManager();
				protected:
				public:
				};
			}
		}
	}
}