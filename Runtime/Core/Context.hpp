#include "../Utility/ExportMacro.hpp"
#include "../Utility/ContructorMacro.hpp"
#include "Manager/ManagerInitializer.hpp"
#include "Manager/ManagerBase.hpp"
#include <vector>
#include <map>
#pragma once

#include <string>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Manager
			{
				class GraphicDeviceManager;
			}
			struct DLL_API Context final
			{
			private:
				static Manager::GraphicDeviceManager* const _graphicDeviceManager;

				static const std::map<std::string, Manager::ManagerBase*> _managerTable;

				NO_CONTRUCTOR(Context)
				NO_COPY(Context)
				NO_MOVE(Context)

			public:
				static void Init();
				static inline Manager::GraphicDeviceManager& GraphicDeviceManager();
			};
		}
	}
}
