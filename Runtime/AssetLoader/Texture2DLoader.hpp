#pragma once
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
					std::vector<std::string> perMipmapLevelTexturePath;
					std::string mipmapGenerateType;
					std::string originalFormat;
					std::string format;
					std::string imageLayout;
					std::vector<std::string> imageUsageFlags;
					std::vector<std::string> memoryPropertyFlags;
					std::vector<std::string> imageAspectFlags;
					bool topDown;

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
						topDown
					)
				};
				Asset::AssetBase* OnLoadAsset(const std::string& path, Utility::Fiber::shared_future<void>& loadOperationFuture, bool& isInLoading) override;
				void OnUnloadAsset(Asset::AssetBase* asset) override;
				static void PopulateTexture2D(AirEngine::Runtime::Asset::Texture2D* texture2d, const std::string path, bool* isInLoading);
			public:
				Texture2DLoader();
				virtual ~Texture2DLoader();
				NO_COPY_MOVE(Texture2DLoader);
			};
		}
	}
}