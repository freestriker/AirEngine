#pragma once
#include "AirEngine/Runtime/Utility/ExportMacro.hpp"
#include "AirEngine/Runtime/Utility/ContructorMacro.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Loader
		{
			class LoadContext;
		}
		namespace Asset
		{
			class AIR_ENGINE_API AssetBase
			{
			private:
				Loader::LoadContext* assetLoadContext;
			public:
				AssetBase() = default;

				virtual ~AssetBase() = default;
				NO_COPY_MOVE(AssetBase);
			};
		}
	}
}