#pragma once
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/ContructorMacro.hpp"
#include <functional>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Manager
			{
				class ManagerBase;

				using ManagerInitializer = std::function<void()>;

				struct ManagerInitializerWrapper
				{
					uint32_t initLayer;
					uint32_t initIndex;
					ManagerInitializer initializer;
				};
			}
		}
	}
}

