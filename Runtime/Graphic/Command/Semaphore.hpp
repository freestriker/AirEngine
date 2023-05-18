#pragma once
#include <vulkan/vulkan_core.h>
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Command
			{
				class AIR_ENGINE_API Semaphore final
				{
				private:
					VkSemaphore _vkSemaphore;
				public:
					Semaphore();
					~Semaphore();
					NO_COPY_MOVE(Semaphore)

					inline VkSemaphore& VkHandle()
					{
						return _vkSemaphore;
					}
				};
			}
		}
	}
}