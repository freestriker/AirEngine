#pragma once
#include "../Utility/ExportMacro.hpp"
#include <string>
#include "../Utility/Fiber.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Asset
		{
			class AssetBase;
		}
		namespace AssetLoader
		{
			class AssetLoaderBase;
			struct AIR_ENGINE_API AssetLoadContext
			{
				std::string path;
				using PathHashValue = size_t;
				PathHashValue pathHash;
				Utility::Fiber::shared_future<void> loadOperationFuture;
				bool isInLoading;
				Asset::AssetBase* asset;
				uint32_t referenceCount;
				AssetLoaderBase* assetLoader;
			};
		}
	}
}