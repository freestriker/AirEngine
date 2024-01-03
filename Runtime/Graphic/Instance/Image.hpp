#pragma once
#include <vulkan/vulkan.hpp>
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
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
				class 
					Image final
				{
				private:
					vk::Format _format;
					vk::Extent3D _extent3D;
					vk::ImageType _imageType;
					vk::ImageTiling _imageTiling;
					vk::ImageUsageFlags _imageUsageFlags;
					vk::ImageCreateFlags _imageCreateFlags;
					uint32_t _layerCount;
					uint32_t _mipmapLevelCount;
					vk::Image _image;
					bool _isNative;
					std::shared_ptr<Memory> _memory;
				public:
					Image(
						vk::Format format,
						vk::Extent3D extent3D,
						vk::ImageType imageType,
						uint32_t layerCount,
						uint32_t mipmapLevelCount,
						vk::ImageUsageFlags imageUsageFlags,
						vk::MemoryPropertyFlags property,
						vk::ImageTiling imageTiling = vk::ImageTiling::eOptimal,
						vk::ImageCreateFlags imageCreateFlags = {},
						VmaAllocationCreateFlags flags = 0, VmaMemoryUsage memoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO
					);
					Image(
						vk::Format format,
						vk::Extent3D extent3D,
						vk::ImageType imageType,
						uint32_t layerCount,
						uint32_t mipmapLevelCount,
						vk::ImageUsageFlags imageUsageFlags,
						std::shared_ptr<Memory> memory,
						vk::ImageTiling imageTiling = vk::ImageTiling::eOptimal,
						vk::ImageCreateFlags imageCreateFlags = {}
					);
					Image(
						vk::Format format,
						vk::Extent3D extent3D,
						vk::ImageType imageType,
						uint32_t layerCount,
						uint32_t mipmapLevelCount,
						vk::ImageUsageFlags imageUsageFlags,
						vk::ImageTiling imageTiling = vk::ImageTiling::eOptimal,
						vk::ImageCreateFlags imageCreateFlags = {}
					);
					Image(
						vk::Image image,
						vk::Format format,
						vk::Extent3D extent3D,
						vk::ImageType imageType,
						uint32_t layerCount,
						uint32_t mipmapLevelCount,
						vk::ImageUsageFlags imageUsageFlags,
						vk::ImageTiling imageTiling = vk::ImageTiling::eOptimal,
						vk::ImageCreateFlags imageCreateFlags = {}
					);
					static Image* CreateSwapchainImage(
						vk::Image image,
						vk::Format format,
						vk::Extent2D extent2D,
						vk::ImageUsageFlags imageUsageFlags
					);
					~Image();
					inline vk::Image VkHandle() const
					{
						return _image;
					}
					inline vk::Format Format() const
					{
						return _format;
					}
					inline vk::Extent3D Extent3D() const
					{
						return _extent3D;
					}
					inline vk::Extent2D Extent2D() const
					{
						return { _extent3D.width, _extent3D.height };
					}
					inline vk::ImageType ImageType() const
					{
						return _imageType;
					}
					inline vk::ImageTiling ImageTiling() const
					{
						return _imageTiling;
					}
					inline vk::ImageUsageFlags ImageUsageFlags() const
					{
						return _imageUsageFlags;
					}
					inline vk::ImageCreateFlags ImageCreateFlags() const
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
					static std::vector<vk::Extent3D> GetPerMipmapLevelExtent3D(const vk::Extent3D& extent, uint32_t mipMapLevelCount)
					{
						std::vector<vk::Extent3D> pmlImageExtent(mipMapLevelCount);
						pmlImageExtent[0] = extent;
						for (int mipMapLevelIndex = 1; mipMapLevelIndex < mipMapLevelCount; mipMapLevelIndex++)
						{
							auto width = pmlImageExtent[mipMapLevelIndex - 1].width / 2;
							width = std::max(1u, width);
							auto height = pmlImageExtent[mipMapLevelIndex - 1].height / 2;
							height = std::max(1u, height);
							auto depth = pmlImageExtent[mipMapLevelIndex - 1].depth / 2;
							depth = std::max(1u, depth);
							vk::Extent3D extent = { width, height, depth };
							pmlImageExtent[mipMapLevelIndex] = extent;
						}
						return pmlImageExtent;
					}
					inline std::vector<vk::Extent3D> PerMipmapLevelExtent3D()const
					{
						return GetPerMipmapLevelExtent3D(_extent3D, _mipmapLevelCount);
					}
					

					NO_COPY_MOVE(Image)
				};
			}
		}
	}
}