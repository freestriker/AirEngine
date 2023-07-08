#pragma once
#include "../Utility/ExportMacro.hpp"
#include "../Utility/ContructorMacro.hpp"
#include "../Utility/CollectableObject.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace AssetLoader
		{
			class AssetLoadContext;
		}
		namespace Asset
		{
			class AIR_ENGINE_API AssetBase
				: public Utility::CollectableObject
			{
			private:
				AssetLoader::AssetLoadContext* assetLoadContext;
			public:
				AssetBase() = default;

				virtual ~AssetBase() = default;
				NO_COPY_MOVE(AssetBase);
			};
		}
	}
}