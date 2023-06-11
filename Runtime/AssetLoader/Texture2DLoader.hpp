﻿#pragma once
#include "AssetLoaderBase.hpp"
#include <nlohmann/json.hpp>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Asset
		{
			class Texture2D;
		}
		namespace AssetLoader
		{
			class AIR_ENGINE_API Texture2DLoader final
				: public AssetLoaderBase
			{
			private:
				struct Descriptor
				{
					std::string texturePath;
					std::string originalFormat;
					std::string format;
					std::string imageLayout;
					std::vector<std::string> imageUsageFlags;
					std::vector<std::string> memoryPropertyFlags;
					std::vector<std::string> imageAspectFlags;
					bool autoGenerateMipmap;
					bool topDown;

					NLOHMANN_DEFINE_TYPE_INTRUSIVE(
						Descriptor,
						texturePath,
						originalFormat,
						format,
						imageLayout,
						imageUsageFlags,
						memoryPropertyFlags,
						imageAspectFlags,
						autoGenerateMipmap,
						topDown
					)
				};
				Asset::AssetBase* OnLoadAsset(const std::string& path, Utility::Fiber::shared_future<void>& loadOperationFuture, bool& isInLoading) override;
				void OnUnloadAsset(Asset::AssetBase* asset) override;
				static void PopulateTexture2D(AirEngine::Runtime::Asset::Texture2D* texture2d, const std::string path, bool* isInLoading);
			public:
				Texture2DLoader()
					: AssetLoaderBase("texture2d")
				{

				}
				virtual ~Texture2DLoader() = default;
				NO_COPY_MOVE(Texture2DLoader);
			};
		}
	}
}