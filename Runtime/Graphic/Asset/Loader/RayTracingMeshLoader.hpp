#pragma once
#include "AirEngine/Runtime/Asset/Loader/LoaderBase.hpp"
#include <nlohmann/json.hpp>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Graphic
		{
			namespace Asset
			{
				class Mesh;
				namespace Loader
				{
					class AIR_ENGINE_API RayTracingMeshLoader final
						: public AirEngine::Runtime::Asset::Loader::LoaderBase
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
						AirEngine::Runtime::Asset::AssetBase* OnLoadAsset(const std::string& path, std::shared_future<void>& loadOperationFuture, bool& isInLoading) override;
						void OnUnloadAsset(AirEngine::Runtime::Asset::AssetBase* asset) override;
						static void PopulateMesh(AirEngine::Runtime::Graphic::Asset::Mesh* mesh, const std::string path, bool* isInLoading);
					public:
						RayTracingMeshLoader();
						virtual ~RayTracingMeshLoader();
						NO_COPY_MOVE(RayTracingMeshLoader);
					};
				}
			}
		}
	}
}