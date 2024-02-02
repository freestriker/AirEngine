#pragma once
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"
#include "LoadContext.hpp"
#include <unordered_set>
#include "LoadContext.hpp"
#include "LoadHandle.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include "AirEngine/Runtime/Asset/AssetBase.hpp"
#include <optional>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Asset
		{
			namespace Loader
			{
				class AIR_ENGINE_API LoaderBase
				{
				private:
					using AssetSet = std::unordered_set<Asset::AssetBase*>;
					AssetSet _gcHoldAssetSet;
					std::unordered_map<LoadContext::PathHashValue, std::unique_ptr<LoadContext>> _assetLoadContextMap;
					std::mutex _loaderMutex;
					const std::string _supportedSuffixName;
					const std::string _name;
					bool _isReadyToLoad;
				protected:
					LoaderBase(const std::string& name, const std::string& supportedSuffixName);
					NO_COPY_MOVE(LoaderBase);
					virtual Asset::AssetBase* OnLoadAsset(const std::string& path, std::shared_future<void>& loadOperationFuture, bool& isInLoading) = 0;
					virtual void OnUnloadAsset(Asset::AssetBase* asset) = 0;
					virtual void OnInitialize()
					{

					}
					virtual void OnFinalize()
					{

					}
				public:
					virtual ~LoaderBase();
					inline const std::string& SupportedSuffixName()const
					{
						return _supportedSuffixName;
					}
					inline const std::string& Name()const
					{
						return _name;
					}
					LoadHandle LoadAsset(const std::string& path);
					void UnloadAsset(const std::string& path);
					void CollectUnloadableAssets();
					void Initialize();
					void Finalize();
				};
			}
		}
	}
}