#include "Material.hpp"
#include "Shader.hpp"
#include "../Manager/DescriptorManager.hpp"
#include "../Instance/UniformBuffer.hpp"

std::unordered_map<AirEngine::Runtime::Utility::InternedString, std::vector<AirEngine::Runtime::Graphic::Rendering::MaterialDescriptorSetMemoryInfo>> AirEngine::Runtime::Graphic::Rendering::Material::PopulateDescriptorSetMemoryInfosMap(const Shader& shader)
{
	const auto& shaderInfo = shader.Info();

	std::unordered_map<
		AirEngine::Runtime::Utility::InternedString, 
		std::vector<AirEngine::Runtime::Graphic::Rendering::MaterialDescriptorSetMemoryInfo>
	> descriptorSetMemoryInfosMap{};

	std::unique_lock<Utility::Fiber::mutex> lock(Manager::DescriptorManager::Mutex());
	
	for (const auto& subShaderInfoPair : shaderInfo.subShaderInfoMap)
	{
		const auto& subpassName = subShaderInfoPair.first;
		const auto& subpassShaderInfo = subShaderInfoPair.second;

		auto& descriptorSetMemoryInfos = descriptorSetMemoryInfosMap.emplace(
			subpassName, 
			std::vector<AirEngine::Runtime::Graphic::Rendering::MaterialDescriptorSetMemoryInfo>(subpassShaderInfo.descriptorSetInfos.size())
		).first->second;

		for (uint32_t descriptorSetInfoIndex = 0; descriptorSetInfoIndex < subpassShaderInfo.descriptorSetInfos.size(); ++descriptorSetInfoIndex)
		{
			const auto& descriptorSetInfo = subpassShaderInfo.descriptorSetInfos.at(descriptorSetInfoIndex);
			auto& descriptorSetMemoryInfo = descriptorSetMemoryInfos.at(descriptorSetInfoIndex);

			uint32_t solidByteCapcity = descriptorSetInfo.solidByteSize;
			uint32_t totalByteCapcity = solidByteCapcity;

			uint32_t dynamicElementCapcity = 0;
			uint32_t dynamicSingleDescriptorByteSize = 0;
			if (descriptorSetInfo.isDynamicByteSize)
			{
				dynamicElementCapcity = 2;
				dynamicSingleDescriptorByteSize = subpassShaderInfo.descriptorInfos.at(descriptorSetInfo.descriptorInfoIndexs.back()).singleDescriptorByteSize;
				totalByteCapcity += dynamicElementCapcity * dynamicSingleDescriptorByteSize;
			}

			auto&& handle = Manager::DescriptorManager::AllocateDescriptorMemory(totalByteCapcity);

			if (descriptorSetInfo.isDynamicByteSize)
			{
				dynamicElementCapcity = (handle.Size() - solidByteCapcity) / dynamicSingleDescriptorByteSize;
			}

			descriptorSetMemoryInfo.dynamicElementCapcity = dynamicElementCapcity;
			descriptorSetMemoryInfo.handle = handle;
		}
	}

	return descriptorSetMemoryInfosMap;
}
void AirEngine::Runtime::Graphic::Rendering::Material::DestroyAllDescriptorSetMemory(const std::unordered_map<AirEngine::Runtime::Utility::InternedString, std::vector<AirEngine::Runtime::Graphic::Rendering::MaterialDescriptorSetMemoryInfo>>& descriptorSetMemoryInfosMap)
{
	std::unique_lock<Utility::Fiber::mutex> lock(Manager::DescriptorManager::Mutex());

	for (const auto& descriptorSetMemoryInfosPair : descriptorSetMemoryInfosMap)
	{
		const auto& descriptorSetMemoryInfos = descriptorSetMemoryInfosPair.second;

		for (const auto& descriptorSetMemoryInfo : descriptorSetMemoryInfos)
		{
			Manager::DescriptorManager::FreeDescriptorMemory(descriptorSetMemoryInfo.handle);
		}
	}
}

AirEngine::Runtime::Graphic::Rendering::Material::Material(const Shader& shader)
	: _shader(&shader)
	, _bindableAssetMap()
	, _descriptorSetMemoryInfosMap(PopulateDescriptorSetMemoryInfosMap(shader))
{

}

AirEngine::Runtime::Graphic::Rendering::Material::~Material()
{
	DestroyAllDescriptorSetMemory(_descriptorSetMemoryInfosMap);
}

void AirEngine::Runtime::Graphic::Rendering::Material::AutoCheckDescriptorSetMemory(MaterialDescriptorSetMemoryInfo& materialDescriptorSetMemoryInfo, const DescriptorInfo& shaderDescriptorInfo, uint32_t desiredIndex)
{
	const auto& shaderDescriptorSetInfo = *shaderDescriptorInfo.descriptorSetInfo;

	if (shaderDescriptorSetInfo.isDynamicByteSize)
	{
		if (desiredIndex >= materialDescriptorSetMemoryInfo.dynamicElementCapcity)
		{
			uint32_t dynamicElementCapcity = std::pow(2u, uint32_t(std::ceil(std::log2(desiredIndex + 1))));
			uint32_t totalByteCapcity = shaderDescriptorSetInfo.solidByteSize + dynamicElementCapcity * shaderDescriptorInfo.singleDescriptorByteSize;

			auto&& handle = Manager::DescriptorManager::ReallocateDescriptorMemory(materialDescriptorSetMemoryInfo.handle, totalByteCapcity);

			dynamicElementCapcity = (handle.Size() - shaderDescriptorSetInfo.solidByteSize) / shaderDescriptorInfo.singleDescriptorByteSize;

			materialDescriptorSetMemoryInfo.dynamicElementCapcity = dynamicElementCapcity;
			materialDescriptorSetMemoryInfo.handle = handle;
		}
	}
	else
	{
		if (desiredIndex >= shaderDescriptorInfo.descriptorCount)
		{
			qFatal("Shader do not have descriptor in this index.");
		}
	}
}

void AirEngine::Runtime::Graphic::Rendering::Material::AddToBindableAssetMap(Utility::InternedString name, uint32_t desiredIndex, MaterialBindableAssetBase* materialBindableAssetBase, std::map<uint64_t, MaterialBindableAssetBase*>& bindableAssetMap)
{
	const uint64_t targetValue = (uint64_t(name.Value()) << 32) | desiredIndex;

	bindableAssetMap.try_emplace(targetValue, materialBindableAssetBase);
}

AirEngine::Runtime::Graphic::Rendering::MaterialBindableAssetBase* AirEngine::Runtime::Graphic::Rendering::Material::GetFromBindableAssetMap(Utility::InternedString name, uint32_t desiredIndex, std::map<uint64_t, MaterialBindableAssetBase*>& bindableAssetMap)
{
	const uint64_t targetValue = (uint64_t(name.Value()) << 32) | desiredIndex;

	auto&& iter = bindableAssetMap.find(targetValue);
	return iter == bindableAssetMap.end() ? nullptr : iter->second;
}

void AirEngine::Runtime::Graphic::Rendering::Material::SetUniformBuffer(Utility::InternedString name, Instance::UniformBuffer* uniformBuffer, uint32_t index)
{
	uint32_t setCount = 0;
	{
		std::unique_lock<Utility::Fiber::mutex> lock(Manager::DescriptorManager::Mutex());

		for (const auto& subpassShaderInfoPair : _shader->Info().subShaderInfoMap)
		{
			const auto& subpassName = subpassShaderInfoPair.first;
			const auto& subpassShaderInfo = subpassShaderInfoPair.second;
			auto& descriptorSetMemoryInfos = _descriptorSetMemoryInfosMap.at(subpassName);

			auto&& iter = subpassShaderInfo.descriptorNameToDescriptorInfoIndexMap.find(name);
			if (iter == subpassShaderInfo.descriptorNameToDescriptorInfoIndexMap.end()) continue;

			const auto& descriptorInfo = subpassShaderInfo.descriptorInfos.at(iter->second);

			if (descriptorInfo.type != vk::DescriptorType::eUniformBuffer)
			{
				qFatal("This name is not a uniform buffer.");
			}

			const auto& descriptorSetInfo = *descriptorInfo.descriptorSetInfo;
			auto& descriptorSetMemoryInfo = descriptorSetMemoryInfos.at(descriptorSetInfo.index);

			AutoCheckDescriptorSetMemory(descriptorSetMemoryInfo, descriptorInfo, index);

			if (uniformBuffer == nullptr)
			{
				Manager::DescriptorManager::ClearHostDescriptorMemory(
					descriptorSetMemoryInfo.handle,
					descriptorInfo.startByteOffsetInDescriptorSet + index * descriptorInfo.singleDescriptorByteSize,
					descriptorInfo.singleDescriptorByteSize
				);
			}
			else
			{
				Manager::DescriptorManager::WriteToHostDescriptorMemory(
					descriptorSetMemoryInfo.handle,
					uniformBuffer->RawDescriptor().data(),
					descriptorInfo.startByteOffsetInDescriptorSet + index * descriptorInfo.singleDescriptorByteSize,
					descriptorInfo.singleDescriptorByteSize
				);
			}
			++setCount;
		}
	}

	if (setCount == 0)
	{
		qFatal("Do not have asset in shader.");
	}

	AddToBindableAssetMap(name, index, uniformBuffer, _bindableAssetMap);
}

AirEngine::Runtime::Graphic::Instance::UniformBuffer* AirEngine::Runtime::Graphic::Rendering::Material::GetUniformBuffer(Utility::InternedString name, uint32_t index)
{
	return static_cast<AirEngine::Runtime::Graphic::Instance::UniformBuffer * >(GetFromBindableAssetMap(name, index, _bindableAssetMap));
}
