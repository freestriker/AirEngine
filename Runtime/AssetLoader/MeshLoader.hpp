#pragma once
#include "AssetLoaderBase.hpp"
#include <nlohmann/json.hpp>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Asset
		{
			class Mesh;
		}
		namespace AssetLoader
		{
			class AIR_ENGINE_API MeshLoader final
				: public AssetLoaderBase
			{
			private:
				struct Descriptor
				{
					std::string meshPath;
					std::vector<std::string> postProcessSteps;
					std::string meshAttributePaser;

					NLOHMANN_DEFINE_TYPE_INTRUSIVE(
						Descriptor,
						meshPath,
						postProcessSteps,
						meshAttributePaser
					)
				};
				Asset::AssetBase* OnLoadAsset(const std::string& path, std::shared_future<void>& loadOperationFuture, bool& isInLoading) override;
				void OnUnloadAsset(Asset::AssetBase* asset) override;
				static void PopulateMesh(AirEngine::Runtime::Asset::Mesh* mesh, const std::string path, bool* isInLoading);
			public:
				MeshLoader();
				virtual ~MeshLoader();
				NO_COPY_MOVE(MeshLoader);
			};
		}
	}
}