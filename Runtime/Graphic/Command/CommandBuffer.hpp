#pragma once
#include <vulkan/vulkan_core.h>
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/InternedString.hpp"
#include <glm/vec4.hpp>

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
					void Reset();
					void BeginRecord(VkCommandBufferUsageFlags flags = 0);
					void EndRecord();
					void AddPipelineBarrier(const Barrier& barrier, VkDependencyFlags dependencyFlags = 0);
					void ClearColorImage(const Instance::Image& image, VkImageLayout imageLayout, const VkClearColorValue& color);
					template<typename TColorChannel>
					void ClearColorImage(const Instance::Image& image, VkImageLayout imageLayout, const glm::vec<4, TColorChannel, glm::defaultp>& color);
				};
				template<typename TColorChannel>
				inline void CommandBuffer::ClearColorImage(const Instance::Image& image, VkImageLayout imageLayout, const glm::vec<4, TColorChannel, glm::defaultp>& color)
				{
					VkClearColorValue vkColor{};
					auto&& vkColorRef = reinterpret_cast<glm::vec<4, TColorChannel, glm::defaultp>&>(vkColor);
					vkColorRef = color;

					ClearColorImage(image, imageLayout, vkColor);
				}
			}
		}
	}
}