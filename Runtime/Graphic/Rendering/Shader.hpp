#pragma once
#include "../../Asset/AssetBase.hpp"
#include "../../Utility/InternedString.hpp"
#include <vulkan/vulkan.hpp>

namespace AirEngine
{
	namespace Runtime
	{
		namespace AssetLoader
		{
			class ShaderLoader;
		}
		namespace Graphic
		{
			namespace Rendering
			{
				class AIR_ENGINE_API Shader final
					: public Asset::AssetBase
				{
					friend class AssetLoader::ShaderLoader;
				public:
					Shader();
					~Shader();
					NO_COPY_MOVE(Shader);
				};
			}
		}
	}
}