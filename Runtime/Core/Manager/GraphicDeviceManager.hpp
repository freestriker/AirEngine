#pragma once
#include "ManagerBase.hpp"
#include <vulkan/vulkan_core.h>
#include "../Boot/ManagerInitializer.hpp"

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
					static VkInstance _vkInstance;
					virtual std::vector<Boot::ManagerInitializerWrapper> OnGetManagerInitializers() override;
					virtual void OnFinishInitialize() override;
				public:
					static inline VkInstance VkInstance()
					{
						return _vkInstance;
					}
					GraphicDeviceManager();
					virtual ~GraphicDeviceManager();
				};
			}
		}
	}
}