#pragma once
#include <vulkan/vulkan_core.h>
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/InternedString.hpp"
#include <map>

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
				class CommandBuffer;
				class AIR_ENGINE_API CommandPool final
				{
				private:
					VkCommandPool _vkCommandPool;
					Instance::Queue* _queue;
					VkCommandPoolCreateFlags _flags;
					std::map<Utility::InternedString, std::unique_ptr<Command::CommandBuffer>> _commandBufferMap;
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

					void Reset();
					inline Command::CommandBuffer& GetCommandBuffer(Utility::InternedString commandBufferName)
					{
						return *(_commandBufferMap[commandBufferName]);
					}
					Command::CommandBuffer& CreateCommandBuffer(Utility::InternedString commandBufferName, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
					void DestroyCommandBuffer(Utility::InternedString commandBufferName);
				};
			}
		}
	}
}