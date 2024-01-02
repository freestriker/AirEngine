#pragma once
#include "ManagerBase.hpp"
#include <taskflow/taskflow.hpp>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Scene
			{
				class Scene;
			}
			namespace Manager
			{
				class AIR_ENGINE_API TaskManager
					: public ManagerBase
				{
				private:
					NO_COPY_MOVE(TaskManager)
				protected:
					static tf::Executor _executor;
					virtual std::vector<Utility::InitializerWrapper> OnGetInternalInitializers() override;
				public:
					inline static tf::Executor& Executor()
					{
						return _executor;
					}
					TaskManager();
					virtual ~TaskManager();
				};
			}
		}
	}
}