#pragma once
#include "AssetBase.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Image;
			}
		}
		namespace AssetLoader
		{
			class Texture2DLoader;
		}
		namespace Asset
		{
			class AIR_ENGINE_API Texture2D final
				: public AssetBase
			{
				friend class AssetLoader::Texture2DLoader;
			private:
				Graphic::Instance::Image* _image;
			public:
				Texture2D()
					: AssetBase()
					, _image(nullptr)
				{

				}
				~Texture2D()
				{
					delete _image;
					_image = nullptr;
				}
				NO_COPY_MOVE(Texture2D);

				Graphic::Instance::Image& Image()
				{
					return *_image;
				}

				RTTR_ENABLE(AssetBase)
			};
		}
	}
}