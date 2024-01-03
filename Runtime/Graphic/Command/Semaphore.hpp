#pragma once
#include <vulkan/vulkan.hpp>
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"

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
					vk::Semaphore _vkSemaphore;
				public:
					Semaphore();
					~Semaphore();
					NO_COPY_MOVE(Semaphore)

					inline vk::Semaphore& VkHandle()
					{
						return _vkSemaphore;
					}
				};
			}
		}
	}
}