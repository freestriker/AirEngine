#include "Component.hpp"
#include <rttr/registration>
#include "SceneObject.hpp"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Runtime::Core::Scene::Component>("AirEngine::Runtime::Core::Scene::Component");
}

AirEngine::Runtime::Core::Scene::Component::Component()
	: Component(true)
{
}

AirEngine::Runtime::Core::Scene::Component::Component(bool active)
	: Component(false, false, false, active)
{
}

AirEngine::Runtime::Core::Scene::Component::Component(bool isSceneDependent, bool isSceneObjectDependent, bool isPositionDependent, bool active)
	: _componentMetaData(
		IF_SET_BITS(isSceneDependent, 0, IS_SCENE_DEPENDENT_BITS) | 
		IF_SET_BITS(isSceneObjectDependent, 0, IS_SCENE_OBJECT_DEPENDENT_BITS) | 
		IF_SET_BITS(isPositionDependent, 0, IS_POSITION_DEPENDENT_BITS) | 
		IF_SET_BITS(active, 0, IS_ACTIVE_BITS)
	)
	, _sceneObject(nullptr)
{
}

void AirEngine::Runtime::Core::Scene::Component::OnAttachToScene()
{

}

void AirEngine::Runtime::Core::Scene::Component::OnDetachFromScene()
{
}

void AirEngine::Runtime::Core::Scene::Component::OnAttachToSceneObject()
{
}

void AirEngine::Runtime::Core::Scene::Component::OnDetachFromSceneObject()
{
}

void AirEngine::Runtime::Core::Scene::Component::OnSetActive(bool active)
{
}

void AirEngine::Runtime::Core::Scene::Component::OnChangePosition()
{
}