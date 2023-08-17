﻿#pragma once
#include "../../Utility/InternedString.hpp"
#include "../Manager/DescriptorManagerData.hpp"
#include "../../Utility/ExportMacro.hpp"
#include "../../Utility/ContructorMacro.hpp"
#include <unordered_map>
#include "MaterialData.hpp"
#include "ShaderData.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class UniformBuffer;
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
					static void AutoCheckDescriptorSetMemory(MaterialDescriptorSetMemoryInfo& materialDescriptorSetMemoryInfo, const DescriptorInfo& shaderDescriptorInfo, uint32_t desiredIndex);
					static void DestroyAllDescriptorSetMemory(const std::unordered_map<Utility::InternedString, std::vector<MaterialDescriptorSetMemoryInfo>>& descriptorSetMemoryInfosMap);
					static void WriteRawDescriptor(const std::vector<uint8_t>& rawDescriptor, const MaterialDescriptorSetMemoryInfo& materialDescriptorSetMemoryInfo, const DescriptorInfo& shaderDescriptorInfo, uint32_t desiredIndex);
					static void AddToBindableAssetMap(Utility::InternedString, uint32_t desiredIndex, MaterialBindableAssetBase* materialBindableAssetBase, std::map<uint64_t, MaterialBindableAssetBase*>& bindableAssetMap);
					static MaterialBindableAssetBase* GetFromBindableAssetMap(Utility::InternedString, uint32_t desiredIndex, std::map<uint64_t, MaterialBindableAssetBase*>& bindableAssetMap);
				public:
					Material(const Shader& shader);
					void SetUniformBuffer(Utility::InternedString name, Instance::UniformBuffer* uniformBuffer, uint32_t index = 0);
					Instance::UniformBuffer* GetUniformBuffer(Utility::InternedString name, uint32_t index = 0);
					~Material();
					NO_COPY_MOVE(Material);

				};
			}
		}
	}
}