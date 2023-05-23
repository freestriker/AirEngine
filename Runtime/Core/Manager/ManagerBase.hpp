#pragma once

#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/ContructorMacro.hpp"
#include <string>
#include <vector>
#include "../../Utility/Initializer.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			class Bootstrapper;
			namespace Manager
			{
				class AIR_ENGINE_API ManagerBase
				{
					friend class Bootstrapper;
				private:
					const std::string _name;
				protected:
					ManagerBase(const std::string& name);
					virtual ~ManagerBase();
					NO_COPY_MOVE(ManagerBase)

					virtual std::vector<Utility::InitializerWrapper> OnGetManagerInitializers() = 0;
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

