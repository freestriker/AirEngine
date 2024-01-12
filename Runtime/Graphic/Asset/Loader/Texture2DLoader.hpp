﻿#pragma once
#include "AirEngine/Runtime/Asset/Loader/LoaderBase.hpp"
#include <nlohmann/json.hpp>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Asset
			{
				class Texture2D;
				namespace Loader
				{
					class AIR_ENGINE_API Texture2DLoader final
						: public AirEngine::Runtime::Asset::Loader::LoaderBase
					{
					private:
						struct ImageViewDescriptor;
						struct Descriptor
						{
							std::vector<std::string> perMipmapLevelTexturePath;
							std::string mipmapGenerateType;
							std::string originalFormat;
							std::string format;
							std::string imageLayout;
							std::vector<std::string> imageUsageFlags;
							std::vector<std::string> memoryPropertyFlags;
							std::vector<std::string> imageAspectFlags;
							bool topDown;
							std::vector< ImageViewDescriptor> views;

							NLOHMANN_DEFINE_TYPE_INTRUSIVE(
								Descriptor,
								perMipmapLevelTexturePath,
								mipmapGenerateType,
								originalFormat,
								format,
								imageLayout,
								imageUsageFlags,
								memoryPropertyFlags,
								imageAspectFlags,
								topDown,
								views
							)
						};
						struct ImageViewDescriptor
						{
							std::string name;
							std::string layout;
							uint32_t baseMipmapLevel;
							uint32_t mipmapLevelCount;

							NLOHMANN_DEFINE_TYPE_INTRUSIVE(
								ImageViewDescriptor,
								name,
								layout,
								baseMipmapLevel,
								mipmapLevelCount
							)
						};
						AirEngine::Runtime::Asset::AssetBase* OnLoadAsset(const std::string& path, std::shared_future<void>& loadOperationFuture, bool& isInLoading) override;
						void OnUnloadAsset(AirEngine::Runtime::Asset::AssetBase* asset) override;
						static void PopulateTexture2D(AirEngine::Runtime::Graphic::Asset::Texture2D* texture2d, const std::string path, bool* isInLoading);
					public:
						Texture2DLoader();
						virtual ~Texture2DLoader();
						NO_COPY_MOVE(Texture2DLoader);
					};
				}
			}
		}
	}
}