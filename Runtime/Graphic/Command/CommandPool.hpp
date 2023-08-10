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
					vk::CommandPool _vkCommandPool;
					Instance::Queue* _queue;
					vk::CommandPoolCreateFlags _flags;
					std::map<Utility::InternedString, std::unique_ptr<Command::CommandBuffer>> _commandBufferMap;
				public:
					CommandPool(const Utility::InternedString queueName, vk::CommandPoolCreateFlags flags = {});
					~CommandPool();
					NO_COPY_MOVE(CommandPool)

					inline vk::CommandPool& VkHandle()
					{
						return _vkCommandPool;
					}
					inline vk::CommandPoolCreateFlags VkCreateFlags()
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
					Command::CommandBuffer& CreateCommandBuffer(Utility::InternedString commandBufferName, vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
					void DestroyCommandBuffer(Utility::InternedString commandBufferName);
				};
			}
		}
	}
}