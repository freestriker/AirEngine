#pragma once
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/OperationWrapper.hpp"
#include "ManagerBase.hpp"
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <taskflow/taskflow.hpp>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Manager
			{
				class AIR_ENGINE_API MainManager final
					: public ManagerBase
				{
				private:
					std::map<std::string, std::shared_ptr<ManagerBase>> _managerTable;
					tf::Future<void> _mainLoopFuture;
					NO_COPY_MOVE(MainManager)
				
				public:
					MainManager();
					~MainManager() = default;
					void Initialize();
					void Finalize();
					void ResetManager(std::shared_ptr<ManagerBase> manager);
				};
			}
		}
	}
}
