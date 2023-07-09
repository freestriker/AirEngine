#pragma once
#include "AssetLoaderBase.hpp"
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
		}
		namespace AssetLoader
		{
			class AIR_ENGINE_API ShaderLoader final
				: public AssetLoaderBase
			{
			private:
				Asset::AssetBase* OnLoadAsset(const std::string& path, Utility::Fiber::shared_future<void>& loadOperationFuture, bool& isInLoading) override;
				void OnUnloadAsset(Asset::AssetBase* asset) override;
				static void PopulateShader(AirEngine::Runtime::Graphic::Rendering::Shader* shader, const std::string path, bool* isInLoading);
			public:
				ShaderLoader();
				virtual ~ShaderLoader();
				NO_COPY_MOVE(ShaderLoader);
			};
		}
	}
}