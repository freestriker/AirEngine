#include "Scene.hpp"
#include <rttr/registration>

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Runtime::Core::Scene::Scene>("AirEngine::Runtime::Core::Scene::Scene");
}

AirEngine::Runtime::Core::Scene::Scene::Scene(const std::string& name)
	: Object()
	, _name(name)
	, _rootSceneObject("Root")
{
	_rootSceneObject._scene = this;
}

AirEngine::Runtime::Core::Scene::Scene::~Scene()
{
	_rootSceneObject._scene = nullptr;
}