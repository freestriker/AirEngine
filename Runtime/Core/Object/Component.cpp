#include "Component.hpp"
#include <rttr/registration>
#include "Component.hpp"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Runtime::Core::Object::Component>("AirEngine::Runtime::Core::Object::Component");
}

AirEngine::Runtime::Core::Object::Component::Component()
	: Component(false)
{
}

AirEngine::Runtime::Core::Object::Component::Component(bool active)
	: active(active)
	, _sceneObject(nullptr)
{
}

AirEngine::Runtime::Core::Object::Component::Component(bool active, AirEngine::Runtime::Core::Object::SceneObject* sceneObject)
	: active(active)
	, _sceneObject(sceneObject)
{
}

AirEngine::Runtime::Core::Object::SceneObject* AirEngine::Runtime::Core::Object::Component::SceneObject()
{
	return _sceneObject;
}
