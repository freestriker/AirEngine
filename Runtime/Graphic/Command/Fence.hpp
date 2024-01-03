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
				class AIR_ENGINE_API Fence final
				{
				private:
					vk::Fence _vkFence;
				public:
					Fence();
					Fence(bool isSignaled);
					~Fence();
					NO_COPY_MOVE(Fence)

					inline vk::Fence VkHandle() const
					{
						return _vkFence;
					}
					void Reset() const;
					void Wait() const;
					vk::Result Status() const;
				};
			}
		}
	}
}