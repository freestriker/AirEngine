#pragma once
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include <string>
#include <future>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Asset
		{
			class AssetBase;
			namespace Loader
			{
				class LoaderBase;
				struct AIR_ENGINE_API LoadContext
				{
					std::string path;
					using PathHashValue = size_t;
					PathHashValue pathHash;
					std::shared_future<void> loadOperationFuture;
					bool isInLoading;
					Asset::AssetBase* asset;
					uint32_t referenceCount;
					LoaderBase* assetLoader;
				};
			}
		}
	}
}