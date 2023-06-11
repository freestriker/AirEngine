#pragma once
#include "../Utility/ExportMacro.hpp"
#include "../Utility/ContructorMacro.hpp"
#include "../Core/Object.hpp"

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
			struct AIR_ENGINE_API AssetBase
				: public Core::Object
			{
			private:
				AssetLoader::AssetLoadContext* assetLoadContext;
			protected:
				AssetBase();
				virtual ~AssetBase();
				NO_COPY_MOVE(AssetBase);

				RTTR_ENABLE(Core::Object)
			};
		}
	}
}