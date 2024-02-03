﻿#pragma once
#include "AirEngine/Runtime/Asset/Loader/LoaderBase.hpp"
#include <nlohmann/json.hpp>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Rendering
			{
				class Shader;
			}
			namespace Asset::Loader
			{
				class AIR_ENGINE_API ShaderLoader final
					: public AirEngine::Runtime::Asset::Loader::LoaderBase
				{
				private:
					Runtime::Asset::AssetBase* OnLoadAsset(const std::string& path, std::shared_future<void>& loadOperationFuture, bool& isInLoading) override;
					void OnUnloadAsset(Runtime::Asset::AssetBase* asset) override;
					static void PopulateShader(AirEngine::Runtime::Graphic::Rendering::Shader* shader, const std::string path, bool* isInLoading);
					void OnInitialize()override;
				public:
					ShaderLoader();
					virtual ~ShaderLoader();
					NO_COPY_MOVE(ShaderLoader);
				};
			}
		}
	}
}