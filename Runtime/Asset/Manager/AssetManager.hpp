#pragma once
#include "AirEngine/Runtime/Utility/ThreadInclude.hpp"
#include "AirEngine/Runtime/Core/Manager/ManagerBase.hpp"
#include <memory>
#include <unordered_map>
#include "AirEngine/Runtime/Asset/Loader/LoadHandle.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Asset
		{
			namespace Loader
			{
				class LoaderBase;
			}
			namespace Manager
			{
				class AIR_ENGINE_API AssetManager
					: public AirEngine::Runtime::Core::Manager::ManagerBase
				{
				private:
				protected:
					static std::unordered_map<std::string, Loader::LoaderBase*> _nameToAssetLoaderMap;
					static std::unordered_map<std::string, Loader::LoaderBase*> _suffixNameToAssetLoaderMap;
					virtual std::vector<Utility::OperationWrapper> OnGetInitializeOperations() override;
					virtual std::vector<Utility::OperationWrapper> OnGetUpdateOperations() override;
					virtual std::vector<Utility::OperationWrapper> OnGetFinalizeOperations() override;
				public:
					AssetManager();
					virtual ~AssetManager();
					NO_COPY_MOVE(AssetManager)
				protected:
					static void InitializeAllAssetLoader();
					static void CollectUpdate();
					static void FinalizeAllAssetLoader();
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
					static AirEngine::Runtime::Asset::Loader::LoadHandle LoadAsset(const std::string& path);
					static void UnloadAsset(const std::string& path);
					static void CollectAll();
				};
			}
		}
	}
}