#pragma once
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"

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