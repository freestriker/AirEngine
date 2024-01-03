#pragma once
#include <vulkan/vulkan.hpp>
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
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
				class Buffer;
			}
			namespace Command
			{
				class CommandBuffer;
				class AIR_ENGINE_API Barrier final
				{
					friend class CommandBuffer;
				private:
					std::vector<vk::MemoryBarrier2> _memoryBarriers;
					std::vector<vk::BufferMemoryBarrier2> _bufferMemoryBarriers;
					std::vector<vk::ImageMemoryBarrier2> _imageMemoryBarriers;
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
						vk::PipelineStageFlags2 srcStageMask,
						vk::AccessFlags2 srcAccessMask,
						vk::PipelineStageFlags2 dstStageMask,
						vk::AccessFlags2 dstAccessMask,
						vk::ImageLayout oldLayout,
						vk::ImageLayout newLayout,
						vk::ImageAspectFlags aspectMask
					);
					void AddBufferMemoryBarrier(
						const Instance::Buffer& buffer, 
						vk::PipelineStageFlags2 srcStageMask,
						vk::AccessFlags2 srcAccessMask,
						vk::PipelineStageFlags2 dstStageMask,
						vk::AccessFlags2 dstAccessMask
					);
				};
			}
		}
	}
}