#include "AssetLoaderBase.hpp"
#include <city.h>
#include <filesystem>

AirEngine::Runtime::AssetLoader::AssetLoadHandle AirEngine::Runtime::AssetLoader::AssetLoaderBase::LoadAsset(const std::string& path)
{
	std::filesystem::path fsPath(path); 
	auto&& currentPath = std::filesystem::current_path();
	if (!std::filesystem::exists(fsPath)) qFatal("File do not exist.");

	fsPath = std::filesystem::absolute(fsPath);
	AssetLoadContext::PathHashValue pathHash = std::filesystem::hash_value(fsPath);

	std::unique_lock< std::mutex> loaderLock(_loaderMutex);

	AssetLoadContext* assetLoadContext = nullptr;
	auto iter = _assetLoadContextMap.find(pathHash);
	if (iter != _assetLoadContextMap.end())
	{
		assetLoadContext = iter->second.get();

		assetLoadContext->referenceCount++;
	}
	else
	{
		assetLoadContext = new AssetLoadContext{};

		assetLoadContext->path = fsPath.generic_string();
		assetLoadContext->pathHash = pathHash;
		assetLoadContext->referenceCount = 1;
		assetLoadContext->assetLoader = this;
		assetLoadContext->isInLoading = true;
		Asset::AssetBase* asset = OnLoadAsset(assetLoadContext->path, assetLoadContext->loadOperationFuture, assetLoadContext->isInLoading);
		assetLoadContext->asset = asset;

		_assetLoadContextMap.insert(std::make_pair(pathHash, std::move(assetLoadContext)));
		_gcHoldAssetSet.insert(asset);
	}

	AssetLoadHandle handle{};
	handle.assetLoadContext = assetLoadContext;
	return handle;
}

void AirEngine::Runtime::AssetLoader::AssetLoaderBase::UnloadAsset(const std::string& path)
{
	auto&& fsPath = std::filesystem::absolute(std::filesystem::path(path));
	AssetLoadContext::PathHashValue pathHash = std::filesystem::hash_value(fsPath);

	std::unique_lock< std::mutex> loaderLock(_loaderMutex);

	auto iter = _assetLoadContextMap.find(pathHash);
	if (iter != _assetLoadContextMap.end())
	{
		iter->second->referenceCount--;
		if(iter->second->referenceCount < 0)qFatal("Unload too many times.");
	}
	else qFatal("Never load this file.");
}

void AirEngine::Runtime::AssetLoader::AssetLoaderBase::CollectUnloadableAssets()
{
	if (_assetLoadContextMap.size() == 0) return;

	std::unique_lock<std::mutex> loaderLock(_loaderMutex);

	for (auto iter = _assetLoadContextMap.begin(); iter != _assetLoadContextMap.end(); )
	{
		auto&& assetLoadContext = iter->second;

		if (assetLoadContext->referenceCount > 0 || assetLoadContext->isInLoading)
		{
			++iter;
			continue;
		}

		Asset::AssetBase* asset = assetLoadContext->asset;

		_gcHoldAssetSet.erase(asset);
		iter = _assetLoadContextMap.erase(iter);

		OnUnloadAsset(asset);
	}
}

AirEngine::Runtime::AssetLoader::AssetLoaderBase::AssetLoaderBase(const std::string& name, const std::string& supportedSuffixName)
	: _name(name)
	, _gcHoldAssetSet()
	, _assetLoadContextMap()
	, _loaderMutex()
	, _supportedSuffixName(supportedSuffixName)
{
}

AirEngine::Runtime::AssetLoader::AssetLoaderBase::~AssetLoaderBase()
{
}
