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
				class DLL_API GraphicDeviceManager
					: public ManagerBase
				{
				private:
					NO_COPY(GraphicDeviceManager)
					NO_MOVE(GraphicDeviceManager)
				protected:
					virtual std::vector<ManagerInitializerWrapper> OnGetManagerInitializers() override;
					virtual void OnFinishInitialize() override;
				public:
					GraphicDeviceManager();
					virtual ~GraphicDeviceManager();
				};
			}
		}
	}
}