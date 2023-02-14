#pragma once
#include "ManagerBase.hpp"
#include <memory>
#include <rttr/type.h>

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
				class DLL_API SceneManager
					: public ManagerBase
				{
				private:
					NO_COPY_MOVE(SceneManager)
				protected:
					virtual std::vector<ManagerInitializerWrapper> OnGetManagerInitializers() override;
					virtual void OnFinishInitialize() override;
				public:
					SceneManager();
					virtual ~SceneManager();
				protected:
					static std::unordered_map<std::string, Scene::Scene*> _sceneMap;
				public:
					static std::unordered_map<std::string, Scene::Scene*>& SceneMap()
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