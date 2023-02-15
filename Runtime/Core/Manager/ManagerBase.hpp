#pragma once

#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/ContructorMacro.hpp"
#include <string>
#include <vector>
#include "../Boot/ManagerInitializer.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Boot
			{
				class Bootstrapper;
			}
			namespace Manager
			{
				class DLL_API ManagerBase
				{
					friend class Boot::Bootstrapper;
				private:
					const std::string _name;
				protected:
					ManagerBase(const std::string& name);
					virtual ~ManagerBase();
					NO_COPY_MOVE(ManagerBase)

					virtual std::vector<Boot::ManagerInitializerWrapper> OnGetManagerInitializers() = 0;
					virtual void OnFinishInitialize() = 0;
				public:
					const std::string& Name() const
					{
						return _name;
					}
				};
			}
		}
	}
}

