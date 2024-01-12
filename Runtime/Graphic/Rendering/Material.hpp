#pragma once
#include "AirEngine/Runtime/Utility/InternedString.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DescriptorManagerData.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include <unordered_map>
#include "AirEngine/Runtime/Graphic/Rendering/MaterialData.hpp"
#include "AirEngine/Runtime/Graphic/Rendering/ShaderData.hpp"
#include "AirEngine/Runtime/Graphic/Rendering/MaterialBindableAssetBase.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Buffer;
				class UniformBuffer;
				class ImageSampler;
				class ImageView;
			}
			namespace Rendering
			{
				class Shader;
				class AIR_ENGINE_API Material final
				{
				private:
					const Shader* _shader;
					std::map<uint64_t, MaterialBindableAssetBase*> _bindableAssetMap;
					std::unordered_map<Utility::InternedString, std::vector<MaterialDescriptorSetMemoryInfo>> _descriptorSetMemoryInfosMap;
				private:
					static std::unordered_map<Utility::InternedString, std::vector<MaterialDescriptorSetMemoryInfo>> PopulateDescriptorSetMemoryInfosMap(const Shader& shader);
					static void DestroyAllDescriptorSetMemory(const std::unordered_map<Utility::InternedString, std::vector<MaterialDescriptorSetMemoryInfo>>& descriptorSetMemoryInfosMap);
					static void AutoCheckDescriptorSetMemory(MaterialDescriptorSetMemoryInfo& materialDescriptorSetMemoryInfo, const DescriptorInfo& shaderDescriptorInfo, uint32_t desiredIndex);
					
					static void AddToBindableAssetMap(Utility::InternedString, uint32_t desiredIndex, MaterialBindableAssetBase* materialBindableAssetBase, std::map<uint64_t, MaterialBindableAssetBase*>& bindableAssetMap);
					static MaterialBindableAssetBase* GetFromBindableAssetMap(Utility::InternedString, uint32_t desiredIndex, std::map<uint64_t, MaterialBindableAssetBase*>& bindableAssetMap);
					void SetDescriptorData(Utility::InternedString name, MaterialBindableAssetBase* bindableAsset, uint32_t index, vk::DescriptorType descriptorType);
				public:
					Material(const Shader& shader);
					~Material();
					NO_COPY_MOVE(Material);

					void SetUniformBuffer(Utility::InternedString name, Instance::Buffer* uniformBuffer, uint32_t index = 0);
					Instance::Buffer* GetUniformBuffer(Utility::InternedString name, uint32_t index = 0);
					void SetSampledImage(Utility::InternedString name, Instance::ImageView* sampledImageView, uint32_t index = 0);
					Instance::ImageView* GetSampledImage(Utility::InternedString name, uint32_t index = 0);
					void SetImageSampler(Utility::InternedString name, Instance::ImageSampler* imageSampler, uint32_t index = 0);
					Instance::ImageSampler* GetImageSampler(Utility::InternedString name, uint32_t index = 0);
				};
			}
		}
	}
}