#pragma once
#include <future>
#include "LoadContext.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Asset
		{
			namespace Loader
			{
				struct AIR_ENGINE_API LoadHandle
				{
					friend class LoaderBase;
				private:
					LoadContext* assetLoadContext = nullptr;
				public:
					inline const std::string& Path() const
					{
						return assetLoadContext->path;
					}
					inline bool IsValid()const
					{
						return assetLoadContext != nullptr;
					}
					inline std::shared_future<void>& SharedFuture()
					{
						return assetLoadContext->loadOperationFuture;
					}
					inline bool IsCompleted()const
					{
						return !assetLoadContext->isInLoading;
					}
					LoaderBase& AssetLoader()
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
}