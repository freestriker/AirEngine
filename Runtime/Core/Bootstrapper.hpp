#pragma once
#include "../Utility/ExportMacro.hpp"
#include "../Utility/ContructorMacro.hpp"
#include "../Utility/Initializer.hpp"
#include "Manager/ManagerBase.hpp"
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
			class AIR_ENGINE_API Bootstrapper final
			{
			private:
				std::map<std::string, std::shared_ptr<Manager::ManagerBase>> _managerTable;

				NO_COPY_MOVE(Bootstrapper)

			public:
				Bootstrapper();
				~Bootstrapper();
				void Boot();
				void ResetManager(std::shared_ptr<Manager::ManagerBase> manager);
			};
		}
	}
}
