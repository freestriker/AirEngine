#pragma once
#include "../../Utility/GarbageCollectInclude.hpp"
#include "ManagerBase.hpp"
#include <memory>
#include <rttr/type.h>
#include "../../Utility/OperationWrapper.hpp"

namespace AirEngine
{
	namespace Runtime
	{
		namespace Core
		{
			namespace Scene
			{
				class Scene;
			}
			namespace Manager
			{
				class AIR_ENGINE_API SceneManager
					: public ManagerBase
				{
				private:
					NO_COPY_MOVE(SceneManager)
				protected:
					virtual std::vector<Utility::OperationWrapper> OnGetInternalInitializers() override;
				public:
					SceneManager();
					virtual ~SceneManager();
				protected:
					static std::unordered_map<std::string, Scene::Scene*, std::hash<std::string>, std::equal_to<std::string>, gc_allocator<std::pair<const std::string, Scene::Scene*>>> _sceneMap;
				public:
					static std::unordered_map<std::string, Scene::Scene*, std::hash<std::string>, std::equal_to<std::string>, gc_allocator<std::pair<const std::string, Scene::Scene*>>>& SceneMap()
					{
						return _sceneMap;
					}
					static Scene::Scene& Scene(const std::string& name);
					static void AddScene(Scene::Scene& scene);
					static void RemoveScene(const Scene::Scene& scene);
				};
			}
		}
	}
}