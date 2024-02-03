#pragma once
#include "AirEngine/Runtime/Asset/AssetBase.hpp"
#include "AirEngine/Runtime/Utility/InternedString.hpp"
#include <vulkan/vulkan.hpp>
#include <spirv_reflect.h>
#include "ShaderData.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Asset::Loader
			{
				class ShaderLoader;
			}
			namespace Instance
			{
				class RenderPassBase;
			}
			namespace Rendering
			{
				class AIR_ENGINE_API Shader final
					: public Runtime::Asset::AssetBase
				{
					friend class Runtime::Graphic::Asset::Loader::ShaderLoader;
				private:
					static uint32_t _shaderGroupHandleSize;
					static uint32_t _shaderGroupHandleAlignedSize;
					ShaderInfo _shaderInfo;
				public:
					Shader();
					~Shader();
					inline const ShaderInfo& Info()const
					{
						return _shaderInfo;
					}
					NO_COPY_MOVE(Shader);
					static inline uint32_t ShaderGroupHandleSize()
					{
						return _shaderGroupHandleSize;
					}
					static inline  uint32_t ShaderGroupHandleAlignedSize()
					{
						return _shaderGroupHandleAlignedSize;
					}
				};
			}
		}
	}
}