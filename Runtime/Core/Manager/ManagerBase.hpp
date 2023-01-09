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
			class Context;
			namespace Manager
			{
				class DLL_API ManagerBase
				{
					friend class Core::Context;
				private:
					const std::string _name;
				protected:
					ManagerBase(const std::string& name);
					virtual ~ManagerBase();
					NO_COPY(ManagerBase)
					NO_MOVE(ManagerBase)

					virtual std::vector<ManagerInitializerWrapper> OnGetManagerInitializers() const = 0;
					virtual void OnFinishInitialize() const = 0;
				public:
					const std::string& Name() const;
				};
			}
		}
	}
}

