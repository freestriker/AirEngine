#include "Material.hpp"
#include "Shader.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DescriptorManager.hpp"
#include "AirEngine/Runtime/Graphic/Instance/UniformBuffer.hpp"
#include "AirEngine/Runtime/Graphic/Instance/Image.hpp"
#include "AirEngine/Runtime/Graphic/Instance/ImageSampler.hpp"
#include "AirEngine/Runtime/Graphic/Instance/ImageView.hpp"

std::unordered_map<AirEngine::Runtime::Utility::InternedString, std::vector<AirEngine::Runtime::Graphic::Rendering::MaterialDescriptorSetMemoryInfo>> AirEngine::Runtime::Graphic::Rendering::Material::PopulateDescriptorSetMemoryInfosMap(const Rendering::Shader& shader)
{
	const auto& shaderInfo = shader.Info();

	std::unordered_map<
		AirEngine::Runtime::Utility::InternedString, 
		std::vector<AirEngine::Runtime::Graphic::Rendering::MaterialDescriptorSetMemoryInfo>
	> descriptorSetMemoryInfosMap{};

	std::unique_lock<std::mutex> lock(Manager::DescriptorManager::Mutex());
	
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
	std::unique_lock<std::mutex> lock(Manager::DescriptorManager::Mutex());

	for (const auto& descriptorSetMemoryInfosPair : descriptorSetMemoryInfosMap)
	{
		const auto& descriptorSetMemoryInfos = descriptorSetMemoryInfosPair.second;

		for (const auto& descriptorSetMemoryInfo : descriptorSetMemoryInfos)
		{
			Manager::DescriptorManager::FreeDescriptorMemory(descriptorSetMemoryInfo.handle);
		}
	}
}

AirEngine::Runtime::Graphic::Rendering::Material::Material(const Rendering::Shader& shader)
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

void AirEngine::Runtime::Graphic::Rendering::Material::SetDescriptorData(Utility::InternedString name, MaterialBindableAssetBase* bindableAsset, uint32_t index, vk::DescriptorType descriptorType)
{
	uint32_t setCount = 0;
	{
		std::unique_lock<std::mutex> lock(Manager::DescriptorManager::Mutex());

		for (const auto& subpassShaderInfoPair : _shader->Info().subShaderInfoMap)
		{
			const auto& subpassName = subpassShaderInfoPair.first;
			const auto& subpassShaderInfo = subpassShaderInfoPair.second;
			auto& descriptorSetMemoryInfos = _descriptorSetMemoryInfosMap.at(subpassName);

			auto&& iter = subpassShaderInfo.descriptorNameToDescriptorInfoIndexMap.find(name);
			if (iter == subpassShaderInfo.descriptorNameToDescriptorInfoIndexMap.end()) continue;

			const auto& descriptorInfo = subpassShaderInfo.descriptorInfos.at(iter->second);

			if (descriptorInfo.type != descriptorType)
			{
				qFatal("This material asset can not bind to this material slot.");
			}

			const auto& descriptorSetInfo = *descriptorInfo.descriptorSetInfo;
			auto& descriptorSetMemoryInfo = descriptorSetMemoryInfos.at(descriptorSetInfo.index);

			AutoCheckDescriptorSetMemory(descriptorSetMemoryInfo, descriptorInfo, index);

			if (bindableAsset == nullptr)
			{
				Manager::DescriptorManager::ClearHostDescriptorMemory(
					descriptorSetMemoryInfo.handle,
					descriptorInfo.startByteOffsetInDescriptorSet + index * descriptorInfo.singleDescriptorByteSize,
					descriptorInfo.singleDescriptorByteSize
				);
			}
			else
			{
				auto&& ptr = Manager::DescriptorManager::GetHostDescriptorMemoryPtr(descriptorSetMemoryInfo.handle);
				bindableAsset->SetDescriptorData(ptr + descriptorInfo.startByteOffsetInDescriptorSet + index * descriptorInfo.singleDescriptorByteSize, descriptorType);
			}
			++setCount;
		}
	}

	if (setCount == 0)
	{
		qFatal("Do not have the same name of slot in this material.");
	}

	AddToBindableAssetMap(name, index, bindableAsset, _bindableAssetMap);
}

void AirEngine::Runtime::Graphic::Rendering::Material::SetUniformBuffer(Utility::InternedString name, Instance::Buffer* uniformBuffer, uint32_t index)
{
	if (uniformBuffer && (uniformBuffer->BufferUsageFlags() | vk::BufferUsageFlagBits::eUniformBuffer) == vk::BufferUsageFlags(0)) qFatal("Can not bind a non uniform buufer to this slot.");

	SetDescriptorData(name, uniformBuffer, index, vk::DescriptorType::eUniformBuffer);
}

AirEngine::Runtime::Graphic::Instance::Buffer* AirEngine::Runtime::Graphic::Rendering::Material::GetUniformBuffer(Utility::InternedString name, uint32_t index)
{
	return dynamic_cast<AirEngine::Runtime::Graphic::Instance::UniformBuffer*>(GetFromBindableAssetMap(name, index, _bindableAssetMap));
}

void AirEngine::Runtime::Graphic::Rendering::Material::SetSampledImage(Utility::InternedString name, Instance::ImageView* sampledImageView, uint32_t index)
{
	if (sampledImageView && (sampledImageView->Image()->ImageUsageFlags() | vk::ImageUsageFlagBits::eSampled) == vk::ImageUsageFlagBits(0)) qFatal("Can not bind a non sampled image to this slot.");

	SetDescriptorData(name, sampledImageView, index, vk::DescriptorType::eSampledImage);
}

AirEngine::Runtime::Graphic::Instance::ImageView* AirEngine::Runtime::Graphic::Rendering::Material::GetSampledImage(Utility::InternedString name, uint32_t index)
{
	return dynamic_cast<AirEngine::Runtime::Graphic::Instance::ImageView*>(GetFromBindableAssetMap(name, index, _bindableAssetMap));
}

void AirEngine::Runtime::Graphic::Rendering::Material::SetImageSampler(Utility::InternedString name, Instance::ImageSampler* imageSampler, uint32_t index)
{
	SetDescriptorData(name, imageSampler, index, vk::DescriptorType::eSampler);
}

AirEngine::Runtime::Graphic::Instance::ImageSampler* AirEngine::Runtime::Graphic::Rendering::Material::GetImageSampler(Utility::InternedString name, uint32_t index)
{
	return dynamic_cast<AirEngine::Runtime::Graphic::Instance::ImageSampler*>(GetFromBindableAssetMap(name, index, _bindableAssetMap));
}
