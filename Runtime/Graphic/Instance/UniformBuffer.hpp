#pragma once
#include "Buffer.hpp"
#include "../Rendering/MaterialBindableAssetBase.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class AIR_ENGINE_API UniformBuffer final
					: public Buffer
					, public Rendering::MaterialBindableAssetBase
				{
				public:
					UniformBuffer(
						vk::DeviceSize size, 
						vk::BufferUsageFlags bufferUsage, 
						vk::MemoryPropertyFlags property, 
						VmaAllocationCreateFlags flags = 0, 
						VmaMemoryUsage memoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO
					);
					~UniformBuffer();
					NO_COPY_MOVE(UniformBuffer)
				};
			}
		}
	}
}