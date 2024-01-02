#include "AssetManager.hpp"
#include "FiberManager.hpp"
#include "../../AssetLoader/Texture2DLoader.hpp"
#include "../../AssetLoader/MeshLoader.hpp"
#include "../../AssetLoader/ShaderLoader.hpp"
#include "../../AssetLoader/AssetLoaderBase.hpp"

std::unordered_map<std::string, AirEngine::Runtime::AssetLoader::AssetLoaderBase*> AirEngine::Runtime::Core::Manager::AssetManager::_nameToAssetLoaderMap{ };
std::unordered_map<std::string, AirEngine::Runtime::AssetLoader::AssetLoaderBase*> AirEngine::Runtime::Core::Manager::AssetManager::_suffixNameToAssetLoaderMap{ };
AirEngine::Runtime::Utility::Fiber::fiber AirEngine::Runtime::Core::Manager::AssetManager::_collectFiber{ };

void AirEngine::Runtime::Core::Manager::AssetManager::AddCollectFiber()
{
	FiberManager::AddFiberInitializers({
		[]()->void
		{
			_collectFiber = std::move(Utility::Fiber::fiber(Collect));
		}
	});
}

void AirEngine::Runtime::Core::Manager::AssetManager::Collect()
{
	while (true)
	{
		Utility::ThisFiber::sleep_for(std::chrono::milliseconds(2000));
		CollectAll();
		std::cout << "Collect.\n";
	}
}

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Core::Manager::AssetManager::OnGetInternalInitializers()
{
	return
	{
		{ 1, 0, AddCollectFiber }
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
