#pragma once

#include "ManagerBase.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Manager
			{
				class DLL_API GraphicDeviceManager final
					: public ManagerBase
				{
				private:
					NO_COPY(GraphicDeviceManager)
					NO_MOVE(GraphicDeviceManager)

					virtual std::vector<ManagerInitializerWrapper> OnGetManagerInitializers() const override;
					virtual void OnFinishInitialize() const override;
				public:
					GraphicDeviceManager();
					~GraphicDeviceManager();
				};
			}
		}
	}
}