#pragma once
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
			namespace Rendering
			{
				class Shader;
				class AIR_ENGINE_API Material final
				{
				private:
					const Shader* _shader;
					std::unordered_multimap<Utility::InternedString, MaterialBindableAssetInfo> _bindableAssetInfoMap;
					std::unordered_map<Utility::InternedString, std::vector<MaterialDescriptorSetMemoryInfo>> _descriptorSetMemoryInfosMap;
				private:
					static std::unordered_map<Utility::InternedString, std::vector<MaterialDescriptorSetMemoryInfo>> PopulateDescriptorSetMemoryInfosMap(const Shader& shader);
					static void AutoCheckDescriptorSetMemory(MaterialDescriptorSetMemoryInfo& materialDescriptorSetMemoryInfo, const DescriptorInfo& shaderDescriptorInfo, uint32_t desiredIndex);
					static void DestroyAllDescriptorSetMemory(const std::unordered_map<Utility::InternedString, std::vector<MaterialDescriptorSetMemoryInfo>>& descriptorSetMemoryInfosMap);
				public:
					Material(const Shader& shader);
					~Material();
					NO_COPY_MOVE(Material);

				};
			}
		}
	}
}