#include "SceneManager.hpp"
#include <iostream>
#include "../Scene/Scene.hpp"

std::unordered_map<std::string, AirEngine::Runtime::Core::Scene::Scene*> AirEngine::Runtime::Core::Manager::SceneManager::_sceneMap{};

std::vector<AirEngine::Runtime::Utility::InitializerWrapper> AirEngine::Runtime::Core::Manager::SceneManager::OnGetManagerInitializers()
{
	return
	{
		{
			1, 0,
			[this]()->void
			{
				_sceneMap["MainScene"] = new Scene::Scene("MainScene");
			}
		}
	};
}

void AirEngine::Runtime::Core::Manager::SceneManager::OnFinishInitialize()
{
	std::cout << "Finish initialize " << Name() << std::endl;
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