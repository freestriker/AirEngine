#pragma once
#include <map>
#include <string>
#include <vulkan/vulkan.hpp>
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/InternedString.hpp"
#include "AirEngine/Runtime/Graphic/Rendering/MaterialBindableAssetBase.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Image;
				class AIR_ENGINE_API ImageView final
					: public Rendering::MaterialBindableAssetBase
				{
					friend class Image;
				private:
					Utility::InternedString _name;
					vk::ImageView _vkImageView;
					vk::ImageLayout _vkImageLayout;
					Image* _image;
					vk::ImageViewType _imageViewType;
					vk::ImageSubresourceRange _imageSubresourceRange;
				public:
					ImageView(
						Image* image,
						Utility::InternedString name,
						vk::ImageViewType type,
						vk::ImageLayout layout,
						vk::ImageAspectFlags aspectMask,
						uint32_t baseMipLevel,
						uint32_t levelCount,
						uint32_t baseArrayLayer,
						uint32_t layerCount
					);
					~ImageView();
					NO_COPY_MOVE(ImageView);

					void SetDescriptorData(uint8_t* targetPtr, vk::DescriptorType descriptorType) override;
					Utility::InternedString Name() const
					{
						return _name;
					}
					inline vk::ImageView VkHandle() const
					{ 
						return _vkImageView;
					}
					inline vk::ImageLayout ImageLayout() const
					{ 
						return _vkImageLayout;
					}
					inline Image* Image() const
					{ 
						return _image;
					}
					inline vk::ImageViewType ImageViewType() const
					{
						return _imageViewType;
					}
					inline vk::ImageAspectFlags ImageAspectFlags() const
					{
						return _imageSubresourceRange.aspectMask;
					}
					inline vk::ImageSubresourceRange ImageSubresourceRange() const
					{
						return _imageSubresourceRange;
					}
					inline uint32_t BaseLayer() const
					{
						return _imageSubresourceRange.baseArrayLayer;
					}
					inline uint32_t LayerCount() const
					{
						return _imageSubresourceRange.layerCount;
					}
					inline uint32_t BaseMipmapLevel() const
					{
						return _imageSubresourceRange.baseMipLevel;
					}
					inline uint32_t MipmapLevelCount() const
					{
						return _imageSubresourceRange.levelCount;
					}
				};
			}
		}
	}
}