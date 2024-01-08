#pragma once
#include "AirEngine/Runtime/Utility/ThreadInclude.hpp"
#include "AirEngine/Runtime/Core/Manager/ManagerBase.hpp"
#include <memory>
#include <unordered_map>
#include "AirEngine/Runtime/AssetLoader/AssetLoadHandle.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace AssetLoader
		{
			class AssetLoaderBase;
		}
		namespace Asset
		{
			namespace Manager
			{
				class AIR_ENGINE_API AssetManager
					: public AirEngine::Runtime::Core::Manager::ManagerBase
				{
				private:
				protected:
					static std::unordered_map<std::string, AssetLoader::AssetLoaderBase*> _nameToAssetLoaderMap;
					static std::unordered_map<std::string, AssetLoader::AssetLoaderBase*> _suffixNameToAssetLoaderMap;
					virtual std::vector<Utility::OperationWrapper> OnGetUpdateOperations() override;
				public:
					AssetManager();
					virtual ~AssetManager();
					NO_COPY_MOVE(AssetManager)
				protected:
					static void CollectUpdate();
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