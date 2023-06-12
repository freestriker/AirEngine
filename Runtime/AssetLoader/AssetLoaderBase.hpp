#pragma once
#include "../Utility/GarbageCollectInclude.hpp"
#include "../Utility/ExportMacro.hpp"
#include "../Utility/ContructorMacro.hpp"
#include "AssetLoadContext.hpp"
#include <unordered_set>
#include "AssetLoadContext.hpp"
#include "AssetLoadHandle.hpp"
#include <unordered_map>
#include "../Utility/Fiber.hpp"
#include <string>
#include <memory>

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
			class AIR_ENGINE_API AssetLoaderBase
			{
			private:
				using AssetSet = std::unordered_set<Asset::AssetBase*, std::hash<Asset::AssetBase*>, std::equal_to<Asset::AssetBase*>, gc_allocator<Asset::AssetBase*>>;
				AssetSet _gcHoldAssetSet;
				std::unordered_map<AssetLoadContext::PathHashValue, std::unique_ptr<AssetLoadContext>> _assetLoadContextMap;
				Utility::Fiber::mutex _loaderMutex;
				const std::string _supportedSuffixName;
				const std::string _name;
			protected:
				AssetLoaderBase(const std::string& name, const std::string& supportedSuffixName);
				virtual ~AssetLoaderBase();
				NO_COPY_MOVE(AssetLoaderBase);
				virtual Asset::AssetBase* OnLoadAsset(const std::string& path, Utility::Fiber::shared_future<void>& loadOperationFuture, bool& isInLoading) = 0;
				virtual void OnUnloadAsset(Asset::AssetBase* asset) = 0;
			public:
				inline const std::string& SupportedSuffixName()const
				{
					return _supportedSuffixName;
				}
				inline const std::string& Name()const
				{
					return _name;
				}
				AssetLoadHandle LoadAsset(const std::string& path);
				void UnloadAsset(const std::string& path);
				void CollectUnloadableAssets();
			};
		}
	}
}