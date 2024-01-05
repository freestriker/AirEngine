#include "SceneManager.hpp"
#include <iostream>
#include "AirEngine/Runtime/Core/Scene/Scene.hpp"

std::unordered_map<std::string, AirEngine::Runtime::Core::Scene::Scene*> AirEngine::Runtime::Core::Manager::SceneManager::_sceneMap{};

std::vector<AirEngine::Runtime::Utility::OperationWrapper> AirEngine::Runtime::Core::Manager::SceneManager::OnGetInitializeOperations()
{
	return
	{
		{
			POST_INITIALIZE_LAYER, 0,
			"Create root scene object.",
			[this]()->void
			{
				_sceneMap["MainScene"] = new Scene::Scene("MainScene");
			}
		}
	};
}

AirEngine::Runtime::Core::Manager::SceneManager::SceneManager()
	: ManagerBase("SceneManager")
{
}

AirEngine::Runtime::Core::Manager::SceneManager::~SceneManager()
{
}

AirEngine::Runtime::Core::Scene::Scene& AirEngine::Runtime::Core::Manager::SceneManager::Scene(const std::string& name)
{
	return *_sceneMap[name];
}

void AirEngine::Runtime::Core::Manager::SceneManager::AddScene(Scene::Scene& scene)
{
	_sceneMap[scene.Name()] = &scene;
}

void AirEngine::Runtime::Core::Manager::SceneManager::RemoveScene(const Scene::Scene& scene)
{
	_sceneMap.erase(_sceneMap.find(scene.Name()));
}