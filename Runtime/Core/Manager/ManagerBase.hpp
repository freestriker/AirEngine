#pragma once

#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/ContructorMacro.hpp"
#include <string>
#include <vector>
#include "ManagerInitializer.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			class Bootstrapper;
			namespace Manager
			{
				class DLL_API ManagerBase
				{
					friend class Core::Bootstrapper;
				private:
					const std::string _name;
				protected:
					ManagerBase(const std::string& name);
					virtual ~ManagerBase();
					NO_COPY(ManagerBase)
					NO_MOVE(ManagerBase)

					virtual std::vector<ManagerInitializerWrapper> OnGetManagerInitializers() = 0;
					virtual void OnFinishInitialize() = 0;
				public:
					const std::string& Name() const;
				};
			}
		}
	}
}

