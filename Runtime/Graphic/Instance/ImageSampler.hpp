#pragma once
#include <vulkan/vulkan.hpp>
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Graphic/Rendering/MaterialBindableAssetBase.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class ImageSampler final
					: public AirEngine::Runtime::Graphic::Rendering::MaterialBindableAssetBase
				{
				private:
					vk::Filter _magFilter;
					vk::Filter _minFilter;
					vk::SamplerMipmapMode _mipmapMode;
					vk::SamplerAddressMode _addressModeU;
					vk::SamplerAddressMode _addressModeV;
					vk::SamplerAddressMode _addressModeW;
					float _minMipmapLevel;
					float _maxMipmapLevel;
					vk::Sampler _sampler;
				public:
					ImageSampler(
						vk::Filter magFilter,
						vk::Filter minFilter,
						vk::SamplerMipmapMode mipmapMode,
						vk::SamplerAddressMode addressModeU,
						vk::SamplerAddressMode addressModeV,
						vk::SamplerAddressMode addressModeW,
						float minMipmapLevel,
						float maxMipmapLevel
					);
					ImageSampler(
						vk::Filter filter,
						vk::SamplerMipmapMode mipmapMode,
						vk::SamplerAddressMode addressMode,
						float mipmapLevel
					);
					ImageSampler(
						vk::Filter filter,
						vk::SamplerMipmapMode mipmapMode,
						vk::SamplerAddressMode addressMode,
						float minMipmapLevel,
						float maxMipmapLevel
					);
					~ImageSampler();
					NO_COPY_MOVE(ImageSampler)		
				private:
					void CreateVulkanInstance();
					void SetDescriptorData(uint8_t* targetPtr, vk::DescriptorType descriptorType) override;
				};
			}
		}
	}
}