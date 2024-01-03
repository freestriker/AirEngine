#pragma once
#include "ManagerBase.hpp"

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
					static void LogicUpdate();
				protected:
					virtual std::vector<Utility::OperationWrapper> OnGetUpdateOperations() override;
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