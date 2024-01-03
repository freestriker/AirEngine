﻿#pragma once
#include "AirEngine/Runtime/Asset/AssetBase.hpp"
#include "AirEngine/Runtime/Utility/InternedString.hpp"
#include <vulkan/vulkan.hpp>
#include <spirv_reflect.h>
#include "ShaderData.hpp"

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
			namespace Instance
			{
				class RenderPassBase;
			}
			namespace Rendering
			{
				class AIR_ENGINE_API Shader final
					: public Asset::AssetBase
				{
					friend class AssetLoader::ShaderLoader;
				private:
					ShaderInfo _shaderInfo;
				public:
					Shader();
					~Shader();
					inline const ShaderInfo& Info()const
					{
						return _shaderInfo;
					}
					NO_COPY_MOVE(Shader);
				};
			}
		}
	}
}