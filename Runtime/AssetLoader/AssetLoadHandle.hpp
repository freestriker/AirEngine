#pragma once
#include "AssetLoadContext.hpp"

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
			struct AIR_ENGINE_API AssetLoadHandle
			{
				friend class AssetLoaderBase;
			private:
				AssetLoadContext* assetLoadContext = nullptr;
			public:
				inline const std::string& Path() const
				{
					return assetLoadContext->path;
				}
				inline bool IsValid()const
				{
					return assetLoadContext != nullptr;
				}
				inline Utility::Fiber::shared_future<void>& SharedFuture()
				{
					return assetLoadContext->loadOperationFuture;
				}
				inline bool IsCompleted()const
				{
					return !assetLoadContext->isInLoading;
				}
				AssetLoaderBase& AssetLoader()
				{
					return *assetLoadContext->assetLoader;
				}
				template<class TAsset>
				inline TAsset& Asset()
				{
					return *static_cast<TAsset*>(assetLoadContext->asset);
				}
			};
		}
	}
}