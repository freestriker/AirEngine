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
				class Image;
			}
			namespace Command
			{
				class CommandPool;
				class Barrier;
				class AIR_ENGINE_API CommandBuffer final
				{
					friend class CommandPool;
				private:
					Utility::InternedString _name;
					VkCommandBuffer _vkCommandBuffer;
					VkCommandBufferLevel _vkCommandBufferLevel;
					CommandPool* _commandPool;

					CommandBuffer(Utility::InternedString commandBufferName, Command::CommandPool* commandPool, VkCommandBufferLevel level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
					~CommandBuffer();
					NO_COPY_MOVE(CommandBuffer)
				public:

					inline VkCommandBuffer VkHandle()
					{
						return _vkCommandBuffer;
					}
					void AddPipelineBarrier(const Barrier& barrier, VkDependencyFlags dependencyFlags = 0);
					void ClearColorImage(const Instance::Image& image, VkImageLayout imageLayout, const VkClearColorValue& color);
				};
			}
		}
	}
}