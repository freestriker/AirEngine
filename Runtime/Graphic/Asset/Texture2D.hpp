#pragma once
#include "AirEngine/Runtime/Asset/AssetBase.hpp"
#include "AirEngine/Runtime/Graphic/Instance/Image.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Loader
			{
				class Texture2DLoader;
			}
			namespace Asset
			{
				class AIR_ENGINE_API Texture2D final
					: public AirEngine::Runtime::Asset::AssetBase
					, public AirEngine::Runtime::Graphic::Instance::Image
				{
					friend class Loader::Texture2DLoader;
				public:
					Texture2D();
					~Texture2D();
					NO_COPY_MOVE(Texture2D);
				};
			}
		}
	}
}