#pragma once
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/Initializer.hpp"
#include "ManagerBase.hpp"
#include <vector>
#include <map>
#include <memory>
#include <string>

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
