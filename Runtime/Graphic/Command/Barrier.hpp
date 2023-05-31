#pragma once
#include <vulkan/vulkan_core.h>
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include <vector>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Image;
			}
			namespace Command
			{
				class CommandBuffer;
				class AIR_ENGINE_API Barrier final
				{
					friend class CommandBuffer;
				private:
					std::vector<VkMemoryBarrier2> _memoryBarriers;
					std::vector<VkBufferMemoryBarrier2> _bufferMemoryBarriers;
					std::vector<VkImageMemoryBarrier2> _imageMemoryBarriers;
				public:
					Barrier();
					~Barrier() = default;
					NO_COPY_MOVE(Barrier)

					inline void Clear()
					{
						_memoryBarriers.clear();
						_bufferMemoryBarriers.clear();
						_imageMemoryBarriers.clear();
					}
					inline void ClearImageMemoryBarriers()
					{
						_imageMemoryBarriers.clear();
					}
					void AddImageMemoryBarrier(
						const Instance::Image& image, 
						VkPipelineStageFlags2 srcStageMask,
						VkAccessFlags2 srcAccessMask,
						VkPipelineStageFlags2 dstStageMask,
						VkAccessFlags2 dstAccessMask,
						VkImageLayout oldLayout,
						VkImageLayout newLayout,
						VkImageAspectFlags aspectMask
					);
				};
			}
		}
	}
}