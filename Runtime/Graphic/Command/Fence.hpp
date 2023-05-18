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
				class AIR_ENGINE_API Fence final
				{
				private:
					VkFence _vkFence;
				public:
					Fence();
					Fence(bool isSignaled);
					~Fence();
					NO_COPY_MOVE(Fence)

					inline VkFence& VkHandle()
					{
						return _vkFence;
					}
					void Reset();
					void Wait();
					VkResult Status();
				};
			}
		}
	}
}