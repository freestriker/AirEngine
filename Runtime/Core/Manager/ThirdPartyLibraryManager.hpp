#pragma once
#include "ManagerBase.hpp"

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
				class AIR_ENGINE_API ThirdPartyLibraryManager
					: public ManagerBase
				{
				private:
					NO_COPY_MOVE(ThirdPartyLibraryManager)
				protected:
					virtual Utility::OperationWrapper OnGetPreInitializeOperation() override;
				public:
					ThirdPartyLibraryManager();
					virtual ~ThirdPartyLibraryManager();
				};
			}
		}
	}
}