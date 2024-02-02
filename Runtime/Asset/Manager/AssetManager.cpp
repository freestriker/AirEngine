﻿#include "AssetManager.hpp"
#include "AirEngine/Runtime/Graphic/Asset/Loader/Texture2DLoader.hpp"
#include "AirEngine/Runtime/Graphic/Asset/Loader/MeshLoader.hpp"
#include "AirEngine/Runtime/Graphic/Asset/Loader/ShaderLoader.hpp"
#include "AirEngine/Runtime/Asset/Loader/LoaderBase.hpp"

std::unordered_map<std::string, AirEngine::Runtime::Asset::Loader::LoaderBase*> AirEngine::Runtime::Asset::Manager::AssetManager::_nameToAssetLoaderMap{ };
std::unordered_map<std::string, AirEngine::Runtime::Asset::Loader::LoaderBase*> AirEngine::Runtime::Asset::Manager::AssetManager::_suffixNameToAssetLoaderMap{ };

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Asset::Manager::AssetManager::OnGetInitializeOperations()
{
	return
	{
		{ GRAPHIC_INITIALIZE_LAYER, GRAPHIC_POST_INITIALIZE_DEVICE_INDEX, "Initialize all asset loader.", InitializeAllAssetLoader}
	};
}

void AirEngine::Runtime::Asset::Manager::AssetManager::InitializeAllAssetLoader()
{
	for (auto& pair : _nameToAssetLoaderMap)
	{
		pair.second->Initialize();
	}
}

void AirEngine::Runtime::Asset::Manager::AssetManager::CollectUpdate()
{
	CollectAll();
}

void AirEngine::Runtime::Asset::Manager::AssetManager::FinalizeAllAssetLoader()
{
	for (auto& pair : _nameToAssetLoaderMap)
	{
		pair.second->Finalize();
	}
}

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Asset::Manager::AssetManager::OnGetUpdateOperations()
{
	return
	{
		{ COLLECT_UPDATE_LAYER, 0, "Collect asset manager's garbage.", CollectUpdate}
	};
}

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Asset::Manager::AssetManager::OnGetFinalizeOperations()
{
	return
	{
		
	};
}

AirEngine::Runtime::Asset::Manager::AssetManager::AssetManager()
	: ManagerBase("AssetManager")
{
	AddAssetLoader<AirEngine::Runtime::Graphic::Asset::Loader::Texture2DLoader>();
	AddAssetLoader<AirEngine::Runtime::Graphic::Asset::Loader::MeshLoader>();
	AddAssetLoader<AirEngine::Runtime::Graphic::Asset::Loader::ShaderLoader>();
}

AirEngine::Runtime::Asset::Manager::AssetManager::~AssetManager()
{
}

void AirEngine::Runtime::Asset::Manager::AssetManager::RemoveAssetLoader(const std::string& name)
{
	auto assetLoader = _nameToAssetLoaderMap[name];
	_nameToAssetLoaderMap.erase(name);
	_suffixNameToAssetLoaderMap.erase(assetLoader->SupportedSuffixName());
	delete assetLoader;
}

AirEngine::Runtime::Asset::Loader::LoadHandle AirEngine::Runtime::Asset::Manager::AssetManager::LoadAsset(const std::string& path)
{
	auto&& suffix = path.substr(path.find_last_of('.') + 1);
	return _suffixNameToAssetLoaderMap[suffix]->LoadAsset(path);
}

void AirEngine::Runtime::Asset::Manager::AssetManager::UnloadAsset(const std::string& path)
{
	auto&& suffix = path.substr(path.find_last_of('.') + 1);
	_suffixNameToAssetLoaderMap[suffix]->UnloadAsset(path);
}

void AirEngine::Runtime::Asset::Manager::AssetManager::CollectAll()
{
	for (const auto& pair : _suffixNameToAssetLoaderMap)
	{
		pair.second->CollectUnloadableAssets();
	}
}
