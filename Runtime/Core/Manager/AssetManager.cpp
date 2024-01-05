#include "AssetManager.hpp"
#include "AirEngine/Runtime/AssetLoader/Texture2DLoader.hpp"
#include "AirEngine/Runtime/AssetLoader/MeshLoader.hpp"
#include "AirEngine/Runtime/AssetLoader/ShaderLoader.hpp"
#include "AirEngine/Runtime/AssetLoader/AssetLoaderBase.hpp"

std::unordered_map<std::string, AirEngine::Runtime::AssetLoader::AssetLoaderBase*> AirEngine::Runtime::Core::Manager::AssetManager::_nameToAssetLoaderMap{ };
std::unordered_map<std::string, AirEngine::Runtime::AssetLoader::AssetLoaderBase*> AirEngine::Runtime::Core::Manager::AssetManager::_suffixNameToAssetLoaderMap{ };

void AirEngine::Runtime::Core::Manager::AssetManager::CollectUpdate()
{
	CollectAll();
}

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Core::Manager::AssetManager::OnGetUpdateOperations()
{
	return
	{
		{ COLLECT_UPDATE_LAYER, 0, "Collect asset manager's garbage.", CollectUpdate}
	};
}

AirEngine::Runtime::Core::Manager::AssetManager::AssetManager()
	: ManagerBase("AssetManager")
{
	AddAssetLoader<AirEngine::Runtime::AssetLoader::Texture2DLoader>();
	AddAssetLoader<AirEngine::Runtime::AssetLoader::MeshLoader>();
	AddAssetLoader<AirEngine::Runtime::AssetLoader::ShaderLoader>();
}

AirEngine::Runtime::Core::Manager::AssetManager::~AssetManager()
{
}

void AirEngine::Runtime::Core::Manager::AssetManager::RemoveAssetLoader(const std::string& name)
{
	auto assetLoader = _nameToAssetLoaderMap[name];
	_nameToAssetLoaderMap.erase(name);
	_suffixNameToAssetLoaderMap.erase(assetLoader->SupportedSuffixName());
	delete assetLoader;
}

AirEngine::Runtime::AssetLoader::AssetLoadHandle AirEngine::Runtime::Core::Manager::AssetManager::LoadAsset(const std::string& path)
{
	auto&& suffix = path.substr(path.find_last_of('.') + 1);
	return _suffixNameToAssetLoaderMap[suffix]->LoadAsset(path);
}

void AirEngine::Runtime::Core::Manager::AssetManager::UnloadAsset(const std::string& path)
{
	auto&& suffix = path.substr(path.find_last_of('.') + 1);
	_suffixNameToAssetLoaderMap[suffix]->UnloadAsset(path);
}

void AirEngine::Runtime::Core::Manager::AssetManager::CollectAll()
{
	for (const auto& pair : _suffixNameToAssetLoaderMap)
	{
		pair.second->CollectUnloadableAssets();
	}
}
