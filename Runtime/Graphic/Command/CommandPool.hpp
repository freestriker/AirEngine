#pragma once
#include <vulkan/vulkan_core.h>
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/InternedString.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Queue;
			}
			namespace Command
			{
				class AIR_ENGINE_API CommandPool final
				{
				private:
					VkCommandPool _vkCommandPool;
					Instance::Queue* _queue;
					VkCommandPoolCreateFlags _flags;
				public:
					CommandPool(const Utility::InternedString queueName, VkCommandPoolCreateFlags flags = 0);
					~CommandPool();
					NO_COPY_MOVE(CommandPool)

					inline VkCommandPool& VkHandle()
					{
						return _vkCommandPool;
					}
					inline VkCommandPoolCreateFlags VkCreateFlags()
					{
						return _flags;
					}
					inline Instance::Queue& Queue()
					{
						return *_queue;
					}
				};
			}
		}
	}
}