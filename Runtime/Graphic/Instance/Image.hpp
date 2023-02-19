#pragma once
#include <vulkan/vulkan_core.h>
#include "../../Utility/ContructorMacro.hpp"
#include "../../Utility/ExportMacro.hpp"
#include <memory>
#include "Memory.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Memory;
				class DLL_API Image final
				{
				private:
					VkFormat _format;
					VkExtent3D _extent3D;
					VkImageType _imageType;
					VkImageTiling _imageTiling;
					VkImageUsageFlags _imageUsageFlags;
					VkImageCreateFlags _imageCreateFlags;
					uint32_t _layerCount;
					uint32_t _mipmapLevelCount;
					VkImage _image;
					bool _isNative;
					std::shared_ptr<Memory> _memory;
				public:
					Image(
						VkFormat format,
						VkExtent3D extent3D,
						VkImageType imageType,
						uint32_t layerCount,
						uint32_t mipmapLevelCount,
						VkImageUsageFlags imageUsageFlags,
						VkMemoryPropertyFlags property,
						VkImageTiling imageTiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
						VkImageCreateFlags imageCreateFlags = 0,
						VmaAllocationCreateFlags flags = 0, VmaMemoryUsage memoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO
					);
					Image(
						VkFormat format,
						VkExtent3D extent3D,
						VkImageType imageType,
						uint32_t layerCount,
						uint32_t mipmapLevelCount,
						VkImageUsageFlags imageUsageFlags,
						std::shared_ptr<Memory> memory,
						VkImageTiling imageTiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
						VkImageCreateFlags imageCreateFlags = 0
					);
					Image(
						VkFormat format,
						VkExtent3D extent3D,
						VkImageType imageType,
						uint32_t layerCount,
						uint32_t mipmapLevelCount,
						VkImageUsageFlags imageUsageFlags,
						VkImageTiling imageTiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
						VkImageCreateFlags imageCreateFlags = 0
					);
					Image(
						VkImage image,
						VkFormat format,
						VkExtent3D extent3D,
						VkImageType imageType,
						uint32_t layerCount,
						uint32_t mipmapLevelCount,
						VkImageUsageFlags imageUsageFlags,
						VkImageTiling imageTiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
						VkImageCreateFlags imageCreateFlags = 0
					);
					static Image* CreateSwapchainImage(
						VkImage image,
						VkFormat format,
						VkExtent2D extent2D,
						VkImageUsageFlags imageUsageFlags
					);
					~Image();
					inline VkImage VkHandle() const
					{
						return _image;
					}
					inline VkFormat Format() const
					{
						return _format;
					}
					inline VkExtent3D Extent3D() const
					{
						return _extent3D;
					}
					inline VkExtent2D Extent2D() const
					{
						return { _extent3D.width, _extent3D.height };
					}
					inline VkImageType ImageType() const
					{
						return _imageType;
					}
					inline VkImageTiling ImageTiling() const
					{
						return _imageTiling;
					}
					inline VkImageUsageFlags ImageUsageFlags() const
					{
						return _imageUsageFlags;
					}
					inline VkImageCreateFlags ImageCreateFlags() const
					{
						return _imageCreateFlags;
					}
					inline uint32_t LayerCount() const
					{
						return _layerCount;
					}
					inline uint32_t MipmapLevelCount() const
					{
						return _mipmapLevelCount;
					}
					void SetMemory(std::shared_ptr<Memory> memory);
					inline std::shared_ptr<Memory> Memory()const
					{
						return _memory;
					}
					void ClearMemory()
					{
						_memory.reset();
					}

					NO_COPY_MOVE(Image)
				};
			}
		}
	}
}