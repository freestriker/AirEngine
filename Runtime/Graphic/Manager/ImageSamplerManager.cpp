#include "ImageSamplerManager.hpp"
#include "AirEngine/Runtime/Graphic/Manager/DeviceManager.hpp"
#include "AirEngine//Runtime/Graphic/Instance/ImageSampler.hpp"
#include <city.h>
#include <array>

std::unordered_map<uint64_t, AirEngine::Runtime::Graphic::Instance::ImageSampler*> AirEngine::Runtime::Graphic::Manager::ImageSamplerManager::_imageSamplerMap{};

constexpr auto supportedFilters = std::to_array({ vk::Filter::eNearest, vk::Filter::eLinear });
constexpr auto supportedMipmapModes = std::to_array({ vk::SamplerMipmapMode::eLinear, vk::SamplerMipmapMode::eNearest });
constexpr auto supportedSamplerAddressModes = std::to_array({ vk::SamplerAddressMode::eClampToEdge, vk::SamplerAddressMode::eMirroredRepeat, vk::SamplerAddressMode::eRepeat });
constexpr auto supportedMipmapLevelPairs = std::to_array<std::pair<float, float>>({ std::pair(0, 1), std::pair(0, 2), std::pair(0, 3), std::pair(0, 4), std::pair(0, 5), std::pair(0, 6), std::pair(0, 7), std::pair(0, 8), std::pair(0, 9), std::pair(0, 10) });

static inline uint64_t CalculateImageSamplerSettingsHash(vk::Filter filter, vk::SamplerMipmapMode mipmapMode, vk::SamplerAddressMode addressMode, float minMipmapLevel, float maxMipmapLevel)
{
	union ImageSamplerSettingsUnion
	{
		vk::Filter filter;
		vk::SamplerMipmapMode mipmapMode;
		vk::SamplerAddressMode addressMode;
		float minMipmapLevel;
		float maxMipmapLevel;
	};

	std::array<ImageSamplerSettingsUnion, 5> settingsBytes{};
	settingsBytes.at(0).filter = filter;
	settingsBytes.at(1).mipmapMode = mipmapMode;
	settingsBytes.at(2).addressMode = addressMode;
	settingsBytes.at(3).minMipmapLevel = minMipmapLevel;
	settingsBytes.at(4).maxMipmapLevel = maxMipmapLevel;

	return CityHash64(reinterpret_cast<char*>(settingsBytes.data()), settingsBytes.size() * sizeof(ImageSamplerSettingsUnion));
}


AirEngine::Runtime::Graphic::Manager::ImageSamplerManager::ImageSamplerManager()
	: ManagerBase("ImageSamplerManager")
{
}

AirEngine::Runtime::Graphic::Manager::ImageSamplerManager::~ImageSamplerManager()
{
}

AirEngine::Runtime::Graphic::Instance::ImageSampler* AirEngine::Runtime::Graphic::Manager::ImageSamplerManager::ImageSampler(vk::Filter filter, vk::SamplerMipmapMode mipmapMode, vk::SamplerAddressMode addressMode, float minMipmapLevel, float maxMipmapLevel)
{
	auto&& hash = CalculateImageSamplerSettingsHash(filter, mipmapMode, addressMode, minMipmapLevel, maxMipmapLevel);
	auto&& iter = _imageSamplerMap.find(hash);

	if (iter == _imageSamplerMap.end()) qFatal("Do not support this type built-in image sampler.");

	return iter->second;
}

void AirEngine::Runtime::Graphic::Manager::ImageSamplerManager::Initialize()
{
	for (const auto& supportedFilter : supportedFilters)
	{
		for (const auto& supportedMipmapMode : supportedMipmapModes)
		{
			for (const auto& supportedSamplerAddressMode : supportedSamplerAddressModes)
			{
				for (const auto& supportedMipmapLevelPair : supportedMipmapLevelPairs)
				{
					auto&& hash = CalculateImageSamplerSettingsHash(supportedFilter, supportedMipmapMode, supportedSamplerAddressMode, supportedMipmapLevelPair.first, supportedMipmapLevelPair.second);
					auto&& sampler = new Instance::ImageSampler(supportedFilter, supportedMipmapMode, supportedSamplerAddressMode, supportedMipmapLevelPair.first, supportedMipmapLevelPair.second);

					_imageSamplerMap.emplace(hash, sampler);
				}
			}
		}
	}
}

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Graphic::Manager::ImageSamplerManager::OnGetInitializeOperations()
{
	return {
		{ GRAPHIC_INITIALIZE_LAYER, GRAPHIC_POST_INITIALIZE_DEVICE_INDEX, "Initialize built-in image samplers.", Initialize }
	};
}

