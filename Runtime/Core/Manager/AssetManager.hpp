#pragma once
#include "../../Utility/ThreadInclude.hpp"
#include "ManagerBase.hpp"
#include <memory>
#include "../../Utility/OperationWrapper.hpp"
#include "../../Utility/Fiber.hpp"
#include <unordered_map>
#include "../../AssetLoader/AssetLoadHandle.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace AssetLoader
		{
			class AssetLoaderBase;
		}
		namespace Core
		{
			namespace Manager
			{
				class AIR_ENGINE_API AssetManager
					: public ManagerBase
				{
				private:
				protected:
					static std::unordered_map<std::string, AssetLoader::AssetLoaderBase*> _nameToAssetLoaderMap;
					static std::unordered_map<std::string, AssetLoader::AssetLoaderBase*> _suffixNameToAssetLoaderMap;
					virtual std::vector<Utility::OperationWrapper> OnGetInternalInitializers() override;
				public:
					AssetManager();
					virtual ~AssetManager();
					NO_COPY_MOVE(AssetManager)
				protected:
					static Utility::Fiber::fiber _collectFiber;
					static void AddCollectFiber();
					static void Collect();
					template<typename TAssetLoader, typename... Args>
					static TAssetLoader& AddAssetLoader(Args&&... args)
					{
						TAssetLoader* assetLoader = new TAssetLoader(args...);
						_nameToAssetLoaderMap[assetLoader->Name()] = assetLoader;
						_suffixNameToAssetLoaderMap[assetLoader->SupportedSuffixName()] = assetLoader;
						return *assetLoader;
					}
					static void RemoveAssetLoader(const std::string& name);
				public:
					template<class TAssetLoader>
					static TAssetLoader& AssetLoader(const std::string& name)
					{
						return *static_cast<TAssetLoader*>(_nameToAssetLoaderMap[name]);
					}
					static AirEngine::Runtime::AssetLoader::AssetLoadHandle LoadAsset(const std::string& path);
					static void UnloadAsset(const std::string& path);
					static void CollectAll();
				};
			}
		}
	}
}