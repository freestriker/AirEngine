#pragma once
#include "AirEngine/Runtime/Utility/InternedString.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DescriptorManagerData.hpp"
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
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
					static void DestroyAllDescriptorSetMemory(const std::unordered_map<Utility::InternedString, std::vector<MaterialDescriptorSetMemoryInfo>>& descriptorSetMemoryInfosMap);
					static void AutoCheckDescriptorSetMemory(MaterialDescriptorSetMemoryInfo& materialDescriptorSetMemoryInfo, const DescriptorInfo& shaderDescriptorInfo, uint32_t desiredIndex);
					
					static void AddToBindableAssetMap(Utility::InternedString, uint32_t desiredIndex, MaterialBindableAssetBase* materialBindableAssetBase, std::map<uint64_t, MaterialBindableAssetBase*>& bindableAssetMap);
					static MaterialBindableAssetBase* GetFromBindableAssetMap(Utility::InternedString, uint32_t desiredIndex, std::map<uint64_t, MaterialBindableAssetBase*>& bindableAssetMap);
				public:
					Material(const Shader& shader);
					~Material();
					NO_COPY_MOVE(Material);

					void SetUniformBuffer(Utility::InternedString name, Instance::UniformBuffer* uniformBuffer, uint32_t index = 0);
					Instance::UniformBuffer* GetUniformBuffer(Utility::InternedString name, uint32_t index = 0);
				};
			}
		}
	}
}